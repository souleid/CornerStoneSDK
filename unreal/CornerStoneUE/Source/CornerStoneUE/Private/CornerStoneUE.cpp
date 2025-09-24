// Copyright Epic Games, Inc. All Rigts Reserved.

#include "CornerStoneUE.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeCounter.h"

#define LOCTEXT_NAMESPACE "FCornerStoneUEModule"

namespace {
  static void* CSUEMalloc(std::size_t size,
    std::uint32_t align) noexcept {
    return FMemory::Malloc(static_cast<SIZE_T>(size),
      static_cast<uint32>(align));
  }

  static void* CSUERealloc(void* ptr,
    std::size_t size,
    std::uint32_t align) noexcept {
    return FMemory::Realloc(ptr,
      static_cast<SIZE_T>(size),
      static_cast<uint32>(align));
  }

  static void CSUEFree(void* ptr) noexcept { FMemory::Free(ptr); }

  struct UEThreadInfo : ThreadInfo {
    FRunnableThread* thread{ nullptr };
    FRunnable* runnable{ nullptr };
    std::atomic<bool> cancel{ false };
  };

  class EntryRunnable final : public FRunnable {
  public:
    using Entry = cornerstone::ThreadEntry;

    EntryRunnable(Entry e, void* a, std::atomic<bool>* c)
      : entry_(e), arg_(a), cancel_(c) {
    }

    virtual uint32 Run() override {
      if (cancel_ && cancel_->load()) return 0u;
      if (entry_) entry_(arg_);
      return 0u;
    }

    virtual void Stop() override {
      if (cancel_) cancel_->store(true);
    }

    virtual void Exit() override {
      if (done_) done_->Trigger();
    }

  private:
    Entry entry_{ nullptr };
    void* arg_{ nullptr };
    FEvent* done_{ nullptr };
    std::atomic<bool>* cancel_{ nullptr };
  };

  static ThreadHandle UECreate(const cornerstone::ThreadDesc& /*d*/) {
    UEThreadInfo* h = new UEThreadInfo();
    return h;
  }

  static bool UERun(ThreadHandle handle,
    cornerstone::ThreadEntry entry,
    void* arg) {
    UEThreadInfo* h = static_cast<UEThreadInfo*>(handle);
    if (!h || !entry) return false;
    if (h->started.load() || h->pre_canceled.load()) return false;

    if (h->pre_canceled.load()) return false;

    EntryRunnable* r = new EntryRunnable(entry, arg, &h->pre_canceled);
    h->runnable = r;
 
    h->thread = FRunnableThread::Create(
      r, TEXT("CornerStoneUEWorker"), 0, TPri_Normal);
    if (!h->thread) {
      delete r;
      h->runnable = nullptr;
      return false;
    }
    h->started.store(true);
    return true;
  }

  static void UECancel(ThreadHandle handle) {
    UEThreadInfo* h = static_cast<UEThreadInfo*>(handle);
    if (!h) return;
    if (!h->started.load()) {
      h->pre_canceled.store(true);
      return;
    }
    if (h->runnable) {
      h->runnable->Stop();
    }
  }

  static void UEJoin(ThreadHandle handle) {
    UEThreadInfo* h = static_cast<UEThreadInfo*>(handle);
    if (!h) return;
    if (h->thread) {
      h->thread->WaitForCompletion();
    }
    h->started.store(false);
  }

  static void UEDestroy(ThreadHandle handle) {
    UEThreadInfo* h = static_cast<UEThreadInfo*>(handle);
    if (!h) return;

    if (h->thread) {
      h->thread->WaitForCompletion();
      delete h->thread;
      h->thread = nullptr;
    }
    if (h->runnable) {
      delete h->runnable;
      h->runnable = nullptr;
    }
    delete h;
  }
}  // namespace

FCornerStoneUEModule& FCornerStoneUEModule::Get() {
	return FModuleManager::LoadModuleChecked<FCornerStoneUEModule>(
		"CornerStoneUE");
}

bool FCornerStoneUEModule::IsAvailable() {
	return FModuleManager::Get().IsModuleLoaded("CornerStoneUE");
}

void FCornerStoneUEModule::StartupModule()
{
	// changed this default value;
	Init(4);

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("CornerStoneUE")->GetBaseDir();

}

void FCornerStoneUEModule::ShutdownModule()
{
	Shutdown();
}

bool FCornerStoneUEModule::Init(std::size_t threads) {
  if (inited_.load()) return true;

  cornerstone::memory::MemoryAllocator mem_alloc;
  mem_alloc.malloc_fn = &CSUEMalloc;
  mem_alloc.realloc_fn = &CSUERealloc;
  mem_alloc.free_fn = &CSUEFree;

  cornerstone::thread::ThreadAllocator th_alloc;
  th_alloc.create_fn = &UECreate;
  th_alloc.run_fn = &UERun;
  th_alloc.cancel_fn = &UECancel;
  th_alloc.join_fn = &UEJoin;
  th_alloc.destroy_fn = &UEDestroy;

  if (!sdk_.Init(threads, &mem_alloc, &th_alloc))
    return false;

  inited_.store(true);
  return true;
}

void FCornerStoneUEModule::Shutdown() {
  if (!inited_.load()) return;
  sdk_.Shutdown();
  inited_.store(false);
}

cornerstone::TaskId FCornerStoneUEModule::Request(TFunction<void()> fn) {
  if (!inited_.load()) return 0;
  return sdk_.Request([g = MoveTemp(fn)]() { g(); });
}

bool FCornerStoneUEModule::CancelById(cornerstone::TaskId id) {
  if (!inited_.load()) return false;
  return sdk_.CancelById(id);
}

void FCornerStoneUEModule::CancelAll() {
  if (!inited_.load()) return;
  sdk_.CancelAll();
}

bool FCornerStoneUEModule::IsRunning() const noexcept {
  return sdk_.IsRunning();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCornerStoneUEModule, CornerStoneUE)
