// Copyright 2025 souleid

#ifndef SRC_THREAD_THREADALLOCATOR_DEFAULT_H_
#define SRC_THREAD_THREADALLOCATOR_DEFAULT_H_

#include <functional>
#include <memory>

#include "src/thread/IThreadAllocator.h"
#include "src/thread/Thread_Default.h"

namespace cornerstone {

// Thread allocator for default C++ platform (std::thread-based).
class ThreadAllocator_Default : public IThreadAllocator {
 public:
  std::unique_ptr<IThread> CreateThread(
      std::function<void()> entry_point) override;
};

}  // namespace cornerstone

#endif  // SRC_THREAD_THREADALLOCATOR_DEFAULT_H_
