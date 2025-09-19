// Copyright 2025 souleid
#ifndef SRC_THREAD_CSTHREAD_API_H_
#define SRC_THREAD_CSTHREAD_API_H_

#include "src/thread/csthreadallocator.h"

namespace cornerstone {
namespace thread {

struct ThreadAllocator {
  CSThreadAllocator::CreateFn create_fn = nullptr;
  CSThreadAllocator::RunFn run_fn = nullptr;
  CSThreadAllocator::CancelFn cancel_fn = nullptr;
  CSThreadAllocator::JoinFn join_fn = nullptr;
  CSThreadAllocator::DestroyFn destroy_fn = nullptr;
};

inline bool SetAllocator(const ThreadAllocator& a) noexcept {
  return CSThreadAllocator::Instance().SetAllocator(
    a.create_fn, a.run_fn, a.cancel_fn, a.join_fn, a.destroy_fn);
}

inline ThreadHandle Create(const ThreadDesc& d) noexcept {
  return CSThreadAllocator::Instance().Create(d);
}
inline bool Run(ThreadHandle h, ThreadEntry e, void* a) noexcept {
  return CSThreadAllocator::Instance().Run(h, e, a);
}
inline void Cancel(ThreadHandle h) noexcept {
  CSThreadAllocator::Instance().Cancel(h);
}
inline void Join(ThreadHandle h) noexcept {
  CSThreadAllocator::Instance().Join(h);
}
inline void Destroy(ThreadHandle h) noexcept {
  CSThreadAllocator::Instance().Destroy(h);
}

}  // namespace thread
}  // namespace cornerstone

#endif  // SRC_THREAD_CSTHREAD_API_H_
