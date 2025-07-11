// Copyright 2025 souleid
#include "src/memory/MemoryAllocator_Default.h"

namespace cornerstone {

void* MemoryAllocator_Default::Allocate(std::size_t size) {
  return std::malloc(size);
}

void MemoryAllocator_Default::Deallocate(void* ptr) {
  std::free(ptr);
}

}  // namespace cornerstone
