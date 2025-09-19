// Copyright 2025 souleid
#include "src/thread/csthreadallocator.h"

#include <atomic>
#include <memory>
#include <new>
#include <thread>
#include <utility>

#include "src/memory/csnewdelete.h"

namespace cornerstone {
namespace {
ThreadHandle StdCreate(const ThreadDesc& /*desc*/) {
  return CSNew<ThreadInfo>();
}

bool StdRun(ThreadInfo* h, ThreadEntry entry, void* arg) {
  if (!h || !entry) return false;

  if (h->started.load() || h->pre_canceled.load()) return false;

  try {
    h->th.reset(CSNew<std::thread>([entry, arg]() { entry(arg); }));
  }
  catch (...) {
    h->th.reset();
    return false;
  }
  h->started.store(true);
  return true;
}

void StdCancel(ThreadInfo* h) {
  if (!h) return;

  if (!h->started.load()) h->pre_canceled.store(true);
}

void StdJoin(ThreadInfo* h) {
  if (!h) return;

  if (h->th && h->th->joinable()) {
    h->th->join();
  }
  h->started.store(false);
}

void StdDestroy(ThreadInfo* h) {
  if (!h) return;

  if (h->th && h->th->joinable()) {
    h->th->join();
  }
  CSDelete(h);
}
}  // namespace

CSThreadAllocator& CSThreadAllocator::Instance() noexcept {
  static CSThreadAllocator g;
  return g;
}

CSThreadAllocator::CSThreadAllocator() noexcept {
  create_fn_ = &StdCreate;
  run_fn_ = &StdRun;
  cancel_fn_ = &StdCancel;
  join_fn_ = &StdJoin;
  destroy_fn_ = &StdDestroy;
}

bool CSThreadAllocator::SetAllocator(CreateFn create_fn,
  RunFn run_fn,
  CancelFn cancel_fn,
  JoinFn join_fn,
  DestroyFn destroy_fn) noexcept {
  if (inited_) return false;

  create_fn_ = create_fn ? create_fn : &StdCreate;
  run_fn_ = run_fn ? run_fn : &StdRun;
  cancel_fn_ = cancel_fn ? cancel_fn : &StdCancel;
  join_fn_ = join_fn ? join_fn : &StdJoin;
  destroy_fn_ = destroy_fn ? destroy_fn : &StdDestroy;

  inited_ = true;
  return true;
}

ThreadHandle CSThreadAllocator::Create(const ThreadDesc& desc) const noexcept {
  return create_fn_ ? create_fn_(desc) : nullptr;
}

bool CSThreadAllocator::Run(ThreadHandle handle,
    ThreadEntry entry, void* arg) const noexcept {
  return (run_fn_ && handle) ? run_fn_(handle, entry, arg) : false;
}

void CSThreadAllocator::Cancel(ThreadHandle handle) const noexcept {
  if (cancel_fn_ && handle) cancel_fn_(handle);
}

void CSThreadAllocator::Join(ThreadHandle handle) const noexcept {
  if (join_fn_ && handle) join_fn_(handle);
}

void CSThreadAllocator::Destroy(ThreadHandle handle) const noexcept {
  if (destroy_fn_ && handle) destroy_fn_(handle);
}
}  // namespace cornerstone
