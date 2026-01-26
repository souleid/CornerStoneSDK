// Copyright 2025 souleid
#ifndef SRC_TASK_TASKMANAGER_H_
#define SRC_TASK_TASKMANAGER_H_

#include <cstdint>
#include <memory>
#include <utility>

#include "src/memory/memoryutils.h"
#include "src/task/task.h"
#include "src/thread/managedthreadpool.h"

namespace cornerstone {
class TaskManager {
 public:
  explicit TaskManager(ManagedThreadPool* pool);
  ~TaskManager() = default;

  template <typename T, typename ResultHandler>
  uint64_t RequestTask(UniquePtr<T> task,
    ResultHandler&& result_handler) {
    if (!task) return 0;
    const uint64_t task_id = task->GetTaskId();

    std::shared_ptr<T> shared_task(std::move(task));

    auto worker_func = [shared_task]() -> int {
      return shared_task->Run();
    };

    auto handler_func = [task_id,
      handler = std::move(result_handler)](int result_code) {
      handler(task_id, result_code);
      };

    return thread_pool_->EnqueueWithResult(std::move(worker_func),
      std::move(handler_func));
  }

  void CancelById(uint64_t id);
  void CancelAll();

 private:
    ManagedThreadPool* thread_pool_;
};
}  // namespace cornerstone

#endif  // SRC_TASK_TASKMANAGER_H_
