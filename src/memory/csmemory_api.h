// Copyright 2025 souleid

#ifndef SRC_MEMORY_CSMEMORY_API_H_
#define SRC_MEMORY_CSMEMORY_API_H_

#include <cstddef>
#include <cstdint>

#include "src/memory/csmemoryallocator.h"
// Copyright 2025 souleid
namespace cornerstone {
namespace memory {

using MallocFn = void* (*)(std::size_t, std::uint32_t);
using ReallocFn = void* (*)(void*, std::size_t, std::uint32_t);
using FreeFn = void (*)(void*);  // NOLINT

struct MemoryAllocator {
  MallocFn  malloc_fn = nullptr;
  ReallocFn realloc_fn = nullptr;
  FreeFn    free_fn = nullptr;
};

inline bool SetAllocator(const MemoryAllocator& a) noexcept {
  return CSMemoryAllocator::Instance()
    .SetAllocator(a.malloc_fn, a.realloc_fn, a.free_fn);
}

inline void* Malloc(std::size_t size, std::uint32_t align) noexcept {
  return CSMemoryAllocator::Instance().Malloc(size, align);
}
inline void* Realloc(void* ptr, std::size_t size,
                     std::uint32_t align) noexcept {
  return CSMemoryAllocator::Instance().Realloc(ptr, size, align);
}
inline void  Free(void* ptr) noexcept {
  CSMemoryAllocator::Instance().Free(ptr);
}

}  // namespace memory
}  // namespace cornerstone

#endif  // SRC_MEMORY_CSMEMORY_API_H_
