// Copyright 2025 souleid
#include "src/platform/PlatformServices_Default.h"

#include <chrono>
#include <cstdio>

namespace cornerstone {

PlatformServices_Default::PlatformServices_Default() = default;

IMemoryAllocator* PlatformServices_Default::GetMemoryAllocator() {
  return &memory_allocator_;
}

IThreadAllocator* PlatformServices_Default::GetThreadAllocator() {
  return &thread_allocator_;
}

uint64_t PlatformServices_Default::GetCurrentTimeMs() {
  return static_cast<uint64_t>(
    std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now().time_since_epoch()).count());
}

void PlatformServices_Default::Log(const char* message) {
  std::printf("[CornerStoneSDK] %s\n", message);
}

}  // namespace cornerstone
