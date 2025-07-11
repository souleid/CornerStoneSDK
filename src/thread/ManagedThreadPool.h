// Copyright 2025 souleid
// ManagedThreadPool provides a task queue with worker threads.

#ifndef SRC_THREAD_MANAGEDTHREADPOOL_H_
#define SRC_THREAD_MANAGEDTHREADPOOL_H_

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <deque>
#include <thread>
#include <unordered_map>
#include <vector>

#include "src/memory/IMemoryAllocator.h"
#include "src/thread/IThreadAllocator.h"
#include "src/thread/IThread.h"

namespace cornerstone {

// ID to track and cancel individual tasks.
using TaskId = std::uint64_t;

// ManagedThreadPool allows asynchronous task execution.
class ManagedThreadPool {
 public:
  explicit ManagedThreadPool(IMemoryAllocator* memory_allocator,
                              IThreadAllocator* thread_allocator);

  ~ManagedThreadPool();

  void Start(std::size_t num_threads);
  void Stop();

  void Enqueue(std::function<void()> task);

  // Template enqueue with result callback.
  template <typename Func, typename ResultCallback>
  TaskId EnqueueWithResult(Func task_func, ResultCallback callback);

  TaskId EnqueueWithId(TaskId id, std::function<void()> task);
  bool CancelById(TaskId id);

 private:
  struct TaskEntry {
    TaskId id{0};
    std::function<void()> task;
  };

  void WorkerLoop(std::size_t worker_index);

  IMemoryAllocator* memory_allocator_;
  IThreadAllocator* thread_allocator_;

  std::vector<std::unique_ptr<IThread>> threads_;
  std::deque<TaskEntry> task_queue_;
  std::unordered_map<TaskId, bool> canceled_tasks_;

  std::mutex queue_mutex_;
  std::condition_variable condition_;
  std::atomic<bool> stop_;
  std::atomic<TaskId> next_task_id_;
};

}  // namespace cornerstone

#include "src/thread/ManagedThreadPool_impl.h"

#endif  // SRC_THREAD_MANAGEDTHREADPOOL_H_
