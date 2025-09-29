// Copyright 2025 souleid

#ifndef SRC_MEMORY_CSMEMORYALLOCATOR_H_
#define SRC_MEMORY_CSMEMORYALLOCATOR_H_

#include <cstddef>
#include <cstdint>

namespace cornerstone {
class CSMemoryAllocator {
 public:
  using MallocFn = void* (*)(std::size_t, std::uint32_t);
  using ReallocFn = void* (*)(void*, std::size_t, std::uint32_t);
  using FreeFn = void (*)(void*);  // NOLINT

  static CSMemoryAllocator& Instance() noexcept;

  bool SetAllocator(MallocFn malloc_fn,
    ReallocFn realloc_fn,
    FreeFn free_fn) noexcept;

  void* Malloc(std::size_t size, std::uint32_t align) const noexcept;
  void* Realloc(void* ptr,
    std::size_t size,
    std::uint32_t align) const noexcept;
  void Free(void* ptr) const noexcept;

 private:
  CSMemoryAllocator() noexcept;
  CSMemoryAllocator(const CSMemoryAllocator&) = delete;
  CSMemoryAllocator& operator=(const CSMemoryAllocator&) = delete;

  MallocFn malloc_fn_;
  ReallocFn realloc_fn_;
  FreeFn free_fn_;
  bool inited_;
};
}  // namespace cornerstone

#endif  // SRC_MEMORY_CSMEMORYALLOCATOR_H_
