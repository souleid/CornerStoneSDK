// Copyright 2025 souleid
#include "src/thread/Thread_Default.h"

#include <memory>
#include <utility>

namespace cornerstone {

Thread_Default::Thread_Default(std::function<void()> entry_point)
    : entry_point_(std::move(entry_point)) {}

Thread_Default::~Thread_Default() {
  if (thread_ && thread_->joinable()) {
    thread_->join();
  }
}

void Thread_Default::Start() {
  thread_ = std::make_unique<std::thread>(entry_point_);
}

void Thread_Default::Join() {
  if (thread_ && thread_->joinable()) {
    thread_->join();
  }
}

}  // namespace cornerstone
