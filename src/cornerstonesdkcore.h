// Copyright 2025 souleid
#ifndef SRC_CORNERSTONESDKCORE_H_
#define SRC_CORNERSTONESDKCORE_H_

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <mutex>

#include "src/memory/csmemory_api.h"
#include "src/thread/csthreadallocator.h"
#include "src/thread/managedthreadpool.h"
#include "src/thread/csthread_api.h"

namespace cornerstone {

class CornerstoneSdkCore {
 public:
  CornerstoneSdkCore();
  ~CornerstoneSdkCore();

  bool Init(std::size_t num_threads,
    const memory::MemoryAllocator* mem_alloc = nullptr,
    const thread::ThreadAllocator* thr_alloc = nullptr);

  // Shuts down the worker pool and clears pending tasks.
  void Shutdown();

  // Enqueue a fire-and-forget task. Returns a TaskId, or 0 on failure.
  TaskId Request(std::function<void()> task);

  // Enqueue a task with a result callback. Works for void/non-void tasks.
  // Example:
  //   auto id = sdk.RequestWithResult(
  //       [](){ return 42; },
  //       [](int v){ /* handle 42 */ });
  template <typename Func, typename ResultCallback>
  TaskId RequestWithResult(Func task_func, ResultCallback callback) {
    if (!IsRunning()) return 0;
    return pool_.EnqueueWithResult(task_func, callback);
  }

  // Cancel a specific task by id. Returns true if removed from queue.
  bool CancelById(TaskId id);

  // Cancel all queued (not yet running) tasks.
  void CancelAll();

  // Returns true if the worker pool is running.
  bool IsRunning() const noexcept;

 private:
  // Installs memory allocator once if provided.
  // Returns true if ok or not provided. False on failure.
  bool InstallMemoryAllocator_(const memory::MemoryAllocator* mem_alloc);

  // Installs thread allocator once if provided.
  // Returns true if ok or not provided. False on failure.
  bool InstallThreadAllocator_(const thread::ThreadAllocator* thr_alloc);

  // Starts internal thread pool with given thread count.
  void StartPool(std::size_t num_threads);

  mutable std::mutex mu_;
  std::atomic<bool> inited_{ false };
  ManagedThreadPool pool_;
};

}  // namespace cornerstone

#endif  // SRC_CORNERSTONESDKCORE_H_
