// Copyright 2025 souleid
// IMemoryAllocator defines an interface for memory allocation strategies.

#ifndef SRC_MEMORY_CSMEMORYALLOCATOR_H_
#define SRC_MEMORY_CSMEMORYALLOCATOR_H_

#include <cstddef>
#include <cstdint>

namespace cornerstone {
  // Memory allocator with function pointer interface, singleton instance.
class CSMemoryAllocator {
 public:
  using MallocFn = void* (*)(std::size_t, std::uint32_t);
  using ReallocFn = void* (*)(void*, std::size_t, std::uint32_t);
  using FreeFn = void (*)(void*);  // NOLINT

  // Returns singleton instance.
  static CSMemoryAllocator& Instance() noexcept;

  // Sets custom allocator functions. Returns false if already set once.
  bool SetAllocator(MallocFn malloc_fn,
    ReallocFn realloc_fn,
    FreeFn free_fn) noexcept;

  // Allocates, reallocates, frees using current functions.
  void* Malloc(std::size_t size, std::uint32_t align) const noexcept;
  void* Realloc(void* ptr,
    std::size_t size,
    std::uint32_t align) const noexcept;
  void Free(void* ptr) const noexcept;

 private:
  // Private constructor with fallback to stdlib.
  CSMemoryAllocator() noexcept;

  // Non-copyable
  CSMemoryAllocator(const CSMemoryAllocator&) = delete;
  CSMemoryAllocator& operator=(const CSMemoryAllocator&) = delete;

  MallocFn malloc_fn_;
  ReallocFn realloc_fn_;
  FreeFn free_fn_;
  bool inited_;
};
}  // namespace cornerstone

#endif  // SRC_MEMORY_CSMEMORYALLOCATOR_H_
