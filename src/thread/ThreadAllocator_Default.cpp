// Copyright 2025 souleid
#include "src/thread/ThreadAllocator_Default.h"

#include <memory>
#include <utility>

namespace cornerstone {

std::unique_ptr<IThread> ThreadAllocator_Default::CreateThread(
    std::function<void()> entry_point) {
  return std::make_unique<Thread_Default>(std::move(entry_point));
}

}  // namespace cornerstone
