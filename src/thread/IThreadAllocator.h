// Copyright 2025 souleid
// IThreadAllocator defines an interface for creating platform-specific threads.

#ifndef SRC_THREAD_ITHREADALLOCATOR_H_
#define SRC_THREAD_ITHREADALLOCATOR_H_

#include <functional>
#include <memory>

#include "src/thread/IThread.h"

namespace cornerstone {

// Interface for creating IThread instances with platform-specific logic.
class IThreadAllocator {
 public:
  virtual ~IThreadAllocator() = default;

  // Creates a new thread with the given entry point.
  virtual std::unique_ptr<IThread> CreateThread(
      std::function<void()> entry_point) = 0;
};

}  // namespace cornerstone

#endif  // SRC_THREAD_ITHREADALLOCATOR_H_
