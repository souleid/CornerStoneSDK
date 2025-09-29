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
#include <unordered_set>
#include <vector>

#include "src/thread/csthreadallocator.h"

namespace cornerstone {

using TaskId = std::uint64_t;

class ManagedThreadPool {
 public:
  ManagedThreadPool();
  ~ManagedThreadPool();

  void Start(std::size_t num_threads);
  void Stop();

  TaskId Enqueue(std::function<void()> task);

  template <typename Func, typename ResultCallback>
  TaskId EnqueueWithResult(Func task_func, ResultCallback callback);

  TaskId EnqueueWithId(TaskId id, std::function<void()> task);
  bool CancelById(TaskId id);
  void CancelAll();

 private:
  struct TaskEntry {
    TaskId id{0};
    std::function<void()> task;
  };

  void WorkerLoop(std::size_t worker_index);

  static void ThreadEntryAdapter(void* arg);

  std::vector<ThreadHandle> threads_;
  std::deque<TaskEntry> task_queue_;
  std::unordered_set<TaskId> canceled_tasks_;

  std::mutex queue_mutex_;
  std::condition_variable condition_;
  std::atomic<bool> stop_;
  std::atomic<TaskId> next_task_id_{0};
};

}  // namespace cornerstone

#include "src/thread/managedthreadpool_impl.h"

#endif  // SRC_THREAD_MANAGEDTHREADPOOL_H_
