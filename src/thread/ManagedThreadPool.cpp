// Copyright 2025 souleid
#include "src/thread/ManagedThreadPool.h"

#include <utility>

namespace cornerstone {

ManagedThreadPool::ManagedThreadPool(IMemoryAllocator* memory_allocator,
                                     IThreadAllocator* thread_allocator)
    : memory_allocator_(memory_allocator),
      thread_allocator_(thread_allocator),
      stop_(false),
      next_task_id_(1) {
}

ManagedThreadPool::~ManagedThreadPool() {
  Stop();
}

void ManagedThreadPool::Start(std::size_t num_threads) {
  for (std::size_t i = 0; i < num_threads; ++i) {
    auto thread = thread_allocator_->CreateThread(
        [this, i]() { WorkerLoop(i); });
    thread->Start();
    threads_.push_back(std::move(thread));
  }
}

void ManagedThreadPool::Stop() {
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    stop_ = true;
  }
  condition_.notify_all();

  for (auto& thread : threads_) {
    thread->Join();
  }
  threads_.clear();
}

void ManagedThreadPool::Enqueue(std::function<void()> task) {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  TaskEntry entry{0, std::move(task)};
  task_queue_.emplace_front(std::move(entry));
  condition_.notify_one();
}

TaskId ManagedThreadPool::EnqueueWithId(
    TaskId id, std::function<void()> task) {
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    task_queue_.emplace_front(TaskEntry{ id, std::move(task) });
  }
  condition_.notify_one();
  return id;
}

bool ManagedThreadPool::CancelById(TaskId id) {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  for (auto it = task_queue_.begin(); it != task_queue_.end(); ++it) {
    if (it->id == id) {
      task_queue_.erase(it);
      return true;
    }
  }
  return false;
}

void ManagedThreadPool::WorkerLoop(std::size_t /*worker_index*/) {
  while (true) {
    TaskEntry entry;
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      condition_.wait(lock, [this]() {
        return stop_ || !task_queue_.empty();
      });

      if (stop_ && task_queue_.empty()) {
        return;
      }

      entry = std::move(task_queue_.front());
      task_queue_.pop_front();
    }

    if (entry.id != 0) {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      if (canceled_tasks_.count(entry.id)) {
        canceled_tasks_.erase(entry.id);
        continue;
      }
    }

    entry.task();
  }
}

}  // namespace cornerstone
