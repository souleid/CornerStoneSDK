// Copyright 2025 souleid
#ifndef SRC_THREAD_CSTHREADALLOCATOR_H_
#define SRC_THREAD_CSTHREADALLOCATOR_H_

// Thread allocator singleton for CornerStone SDK.
#include <atomic>
#include <cstdint>
#include <memory>
#include <thread>
#include <utility>

struct ThreadInfo {
  std::unique_ptr<std::thread> th;
  std::atomic<bool> started{ false };
  std::atomic<bool> pre_canceled{ false };
};

typedef ThreadInfo* ThreadHandle;

namespace cornerstone {
struct ThreadDesc {
  const char* name = nullptr;
  std::uint32_t stack_size = 0;
  int priority = 0;
  std::uint64_t affinity_mask = 0ULL;
};

using ThreadEntry = void (*)(void* user_arg);

class CSThreadAllocator {
 public:
  using CreateFn = ThreadHandle (*)(const ThreadDesc& desc);
  using RunFn = bool (*)(ThreadHandle, ThreadEntry, void* arg);
  using CancelFn = void (*)(ThreadHandle);
  using JoinFn = void (*)(ThreadHandle);
  using DestroyFn = void (*)(ThreadHandle);

  static CSThreadAllocator& Instance() noexcept;

  bool SetAllocator(CreateFn create_fn,
    RunFn run_fn,
    CancelFn cancel_fn,
    JoinFn join_fn,
    DestroyFn destroy_fn) noexcept;

  ThreadHandle Create(const ThreadDesc& desc) const noexcept;
  bool Run(ThreadHandle handle, ThreadEntry entry, void* arg) const noexcept;
  void Cancel(ThreadHandle handle) const noexcept;
  void Join(ThreadHandle handle) const noexcept;
  void Destroy(ThreadHandle handle) const noexcept;

  bool inited() const noexcept { return inited_; }

 private:
  CSThreadAllocator() noexcept;
  CSThreadAllocator(const CSThreadAllocator&) = delete;
  CSThreadAllocator& operator=(const CSThreadAllocator&) = delete;

  CreateFn create_fn_ = nullptr;
  RunFn run_fn_ = nullptr;
  CancelFn cancel_fn_ = nullptr;
  JoinFn join_fn_ = nullptr;
  DestroyFn destroy_fn_ = nullptr;

  bool inited_ = false;
};
}  // namespace cornerstone

#endif  // SRC_THREAD_CSTHREADALLOCATOR_H_
