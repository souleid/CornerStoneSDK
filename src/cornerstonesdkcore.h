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

  void Shutdown();

  TaskId Request(std::function<void()> task);

  // Example:
  //   auto id = sdk.RequestWithResult(
  //       [](){ return 42; },
  //       [](int v){ /* handle 42 */ });
  template <typename Func, typename ResultCallback>
  TaskId RequestWithResult(Func task_func, ResultCallback callback) {
    if (!IsRunning()) return 0;
    return pool_.EnqueueWithResult(task_func, callback);
  }

  bool CancelById(TaskId id);

  void CancelAll();

  bool IsRunning() const noexcept;

 private:
  bool InstallMemoryAllocator_(const memory::MemoryAllocator* mem_alloc);
  bool InstallThreadAllocator_(const thread::ThreadAllocator* thr_alloc);

  void StartPool(std::size_t num_threads);

  mutable std::mutex mu_;
  std::atomic<bool> inited_{ false };
  ManagedThreadPool pool_;
};

}  // namespace cornerstone

#endif  // SRC_CORNERSTONESDKCORE_H_
