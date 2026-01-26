// Copyright 2025 souleid
#include "src/cornerstonesdkcore.h"

#include <utility>

namespace cornerstone {
CornerstoneSdkCore::CornerstoneSdkCore() : thread_pool_(),
task_manager_(&thread_pool_),
inited_(false) {}

CornerstoneSdkCore::~CornerstoneSdkCore() {
  if (inited_.load())
    Shutdown();
}

bool CornerstoneSdkCore::Init(std::size_t num_threads,
  const memory::MemoryAllocator* mem_alloc,
  const thread::ThreadAllocator* thr_alloc) {
  std::lock_guard<std::mutex> lock(mu_);
  if (inited_.load()) return false;

  if (!InstallMemoryAllocator_(mem_alloc)) return false;
  if (!InstallThreadAllocator_(thr_alloc)) return false;

  StartPool(num_threads);
  inited_.store(true);
  return true;
}

void CornerstoneSdkCore::Shutdown() {
  std::lock_guard<std::mutex> lock(mu_);
  if (!inited_.load()) return;
  thread_pool_.Stop();
  inited_.store(false);
}

TaskId CornerstoneSdkCore::Request(std::function<void()> task) {
  if (!IsRunning()) return 0;
  return thread_pool_.Enqueue(std::move(task));
}

bool CornerstoneSdkCore::CancelById(TaskId id) {
  if (!IsRunning()) return false;
  return thread_pool_.CancelById(id);
}

void CornerstoneSdkCore::CancelAll() {
  if (!IsRunning()) return;
  thread_pool_.CancelAll();
}

bool CornerstoneSdkCore::IsRunning() const noexcept {
  return inited_.load();
}

bool CornerstoneSdkCore::InstallMemoryAllocator_(
  const memory::MemoryAllocator* mem_alloc) {
  if (!mem_alloc) return true;
  return memory::SetAllocator(*mem_alloc);
}

bool CornerstoneSdkCore::InstallThreadAllocator_(
  const thread::ThreadAllocator* thr_alloc) {
  if (!thr_alloc) return true;
  return thread::SetAllocator(*thr_alloc);
}

void CornerstoneSdkCore::StartPool(std::size_t num_threads) {
  if (num_threads == 0) num_threads = 1;
  thread_pool_.Start(num_threads);
}

}  // namespace cornerstone
