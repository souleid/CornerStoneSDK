// Copyright 2025 souleid
#include "src/include/CornerStoneSDKCore.h"

#include <utility>

#include "src/memory/IMemoryAllocator.h"

namespace cornerstone {

CornerStoneSDKCore::CornerStoneSDKCore(
    ISDKPlatformServices* platform_services)
    : platform_(platform_services), thread_pool_(nullptr) {}

CornerStoneSDKCore::~CornerStoneSDKCore() {
  Shutdown();
}

void CornerStoneSDKCore::Start(std::size_t thread_count) {
  if (thread_pool_) {
    return;
  }

  auto* mem_alloc = platform_->GetMemoryAllocator();
  auto* thread_alloc = platform_->GetThreadAllocator();

  thread_pool_ = static_cast<ManagedThreadPool*>(
      mem_alloc->Allocate(sizeof(ManagedThreadPool)));

  new (thread_pool_)
      ManagedThreadPool(mem_alloc, thread_alloc);

  thread_pool_->Start(thread_count);
}

void CornerStoneSDKCore::Shutdown() {
  if (thread_pool_) {
    thread_pool_->Stop();
    thread_pool_->~ManagedThreadPool();
    platform_->GetMemoryAllocator()->Deallocate(thread_pool_);
    thread_pool_ = nullptr;
  }
}

bool CornerStoneSDKCore::CancelTask(TaskId id) {
  if (thread_pool_) {
    return thread_pool_->CancelById(id);
  }
  return false;
}

}  // namespace cornerstone
