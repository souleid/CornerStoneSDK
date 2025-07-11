// Copyright 2025 souleid
#include "PlatformServices_Unreal.h"

namespace cornerstone {
PlatformServices_Unreal::PlatformServices_Unreal()
    : thread_allocator_(&memory_allocator_) {}

PlatformServices_Unreal::~PlatformServices_Unreal() {}

IThreadAllocator* PlatformServices_Unreal::GetThreadAllocator() {
  return &thread_allocator_;
}

IMemoryAllocator* PlatformServices_Unreal::GetMemoryAllocator() {
  return &memory_allocator_;
}
}  // namespace cornerstone
