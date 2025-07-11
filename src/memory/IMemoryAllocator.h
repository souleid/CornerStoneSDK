// Copyright 2025 souleid
// IMemoryAllocator defines an interface for memory allocation strategies.

#ifndef SRC_MEMORY_IMEMORYALLOCATOR_H_
#define SRC_MEMORY_IMEMORYALLOCATOR_H_

#include <cstddef>

namespace cornerstone {

// Abstract interface for memory allocators.
class IMemoryAllocator {
 public:
  virtual ~IMemoryAllocator() = default;

  // Allocates memory of given size.
  virtual void* Allocate(std::size_t size) = 0;

  // Deallocates previously allocated memory.
  virtual void Deallocate(void* ptr) = 0;
};

}  // namespace cornerstone

#endif  // SRC_MEMORY_IMEMORYALLOCATOR_H_
