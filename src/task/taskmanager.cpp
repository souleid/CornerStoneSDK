// Copyright 2025 souleid
#include "src/task/taskmanager.h"

#include <utility>

namespace cornerstone {
TaskManager::TaskManager(ManagedThreadPool* pool) : thread_pool_(pool) {
}

void TaskManager::CancelById(uint64_t id) {
  if (thread_pool_) {
    thread_pool_->CancelById(id);
  }
}

void TaskManager::CancelAll() {
  if (thread_pool_) {
    thread_pool_->CancelAll();
  }
}

}  // namespace cornerstone
