// Copyright 2025 souleid
#include "src/thread/managedthreadpool.h"

#include <memory>
#include <utility>

#include "src/thread/csthread_api.h"

namespace cornerstone {

ManagedThreadPool::ManagedThreadPool()
    : stop_(false),
      next_task_id_(1) {
}

ManagedThreadPool::~ManagedThreadPool() {
  Stop();
}

void ManagedThreadPool::Start(std::size_t num_threads) {
  threads_.reserve(num_threads);

  for (std::size_t i = 0; i < num_threads; ++i) {
    ThreadDesc desc{};
    auto handle = thread::Create(desc);
    if (!handle) {
      continue;
    }

    struct Arg {
      ManagedThreadPool* self;
      std::size_t index;
    };
    Arg* a = new Arg{ this, i };

    bool ok = thread::Run(handle, &ManagedThreadPool::ThreadEntryAdapter,
      static_cast<void*>(a));
    if (!ok) {
      thread::Destroy(handle);
      delete a;
      continue;
    }
    threads_.push_back(handle);
  }
}

void ManagedThreadPool::Stop() {
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    stop_.store(true);
  }
  condition_.notify_all();

  for (auto& h : threads_) {
    if (!h) continue;
    thread::Join(h);
    thread::Destroy(h);
  }
  threads_.clear();

  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    task_queue_.clear();
    canceled_tasks_.clear();
  }
}

TaskId ManagedThreadPool::Enqueue(std::function<void()> task) {
  TaskId id = next_task_id_.fetch_add(1);
  return EnqueueWithId(id, std::move(task));
}

TaskId ManagedThreadPool::EnqueueWithId(
    TaskId id, std::function<void()> task) {
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    if (stop_.load()) return 0;
    task_queue_.emplace_back(TaskEntry{ id, std::move(task) });
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

  canceled_tasks_.insert(id);
  return false;
}

void ManagedThreadPool::CancelAll() {
  std::lock_guard<std::mutex> lock(queue_mutex_);
  if (stop_.load()) return;

  for (const auto& e : task_queue_) {
    canceled_tasks_.insert(e.id);
  }
  task_queue_.clear();
}

void ManagedThreadPool::WorkerLoop(std::size_t /*worker_index*/) {
  while (true) {
    TaskEntry entry;
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      condition_.wait(lock, [this]() {
        return stop_.load() || !task_queue_.empty();
        });

      if (stop_.load() && task_queue_.empty()) {
        return;
      }

      entry = std::move(task_queue_.front());
      task_queue_.pop_front();

      if (entry.id != 0) {
        if (canceled_tasks_.erase(entry.id) > 0) {
          continue;  // Skip canceled task.
        }
      }
    }

    if (entry.task) entry.task();
  }
}

void ManagedThreadPool::ThreadEntryAdapter(void* arg) {
  struct Arg {
    ManagedThreadPool* self;
    std::size_t index;
  };
  std::unique_ptr<Arg> holder(static_cast<Arg*>(arg));
  if (!holder || !holder->self) return;
  holder->self->WorkerLoop(holder->index);
}

}  // namespace cornerstone
