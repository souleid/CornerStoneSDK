// Copyright 2025 souleid

#ifndef SRC_MEMORY_MEMORYALLOCATOR_DEFAULT_H_
#define SRC_MEMORY_MEMORYALLOCATOR_DEFAULT_H_

#include <cstdlib>

#include "src/memory/IMemoryAllocator.h"

namespace cornerstone {

// Default memory allocator using malloc/free for general C++ usage.
class MemoryAllocator_Default : public IMemoryAllocator {
 public:
  void* Allocate(std::size_t size) override;
  void Deallocate(void* ptr) override;
};

}  // namespace cornerstone

#endif  // SRC_MEMORY_MEMORYALLOCATOR_DEFAULT_H_
