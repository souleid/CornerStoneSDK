// Copyright 2025 souleid
#include "MemoryAllocator_Unreal.h"

namespace cornerstone {
void* MemoryAllocator_Unreal::Allocate(std::size_t size) {
  return FMemory::Malloc(size);
}

void MemoryAllocator_Unreal::Deallocate(void* ptr) {
  FMemory::Free(ptr);
}
}  // namespace cornerstone
