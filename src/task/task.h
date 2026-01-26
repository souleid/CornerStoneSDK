// Copyright 2025 souleid
#ifndef SRC_TASK_TASK_H_
#define SRC_TASK_TASK_H_

#include <atomic>
#include <cstdint>

namespace cornerstone {
enum class TaskResultCode : int {
    kSuccess = 0,
    kCancelled = 1,
    kGenericError = 100,
    kFileError = 101,
    kNetworkError = 102,
};

class Task {
 public:
  explicit Task(uint64_t task_id) : task_id_(task_id) {}
  virtual ~Task() = default;

  virtual int Run() = 0;

  virtual uint64_t GetTaskId() const { return task_id_; }

  virtual void Cancel() { is_cancelled_.store(true); }
  virtual bool IsCancelled() const { return is_cancelled_.load(); }

 protected:
  uint64_t task_id_;
  std::atomic<bool> is_cancelled_{ false };
};

}  // namespace cornerstone

#endif  // SRC_TASK_TASK_H_
