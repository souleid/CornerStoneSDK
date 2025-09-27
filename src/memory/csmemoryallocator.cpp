// Copyright 2025 souleid
#include "src/memory/csmemoryallocator.h"

#include <cstdlib>  // NOLINT

namespace cornerstone {
namespace {

void* StdMalloc(std::size_t s, std::uint32_t) noexcept {
  return std::malloc(s);
}

void* StdRealloc(void* p,
  std::size_t s,
  std::uint32_t) noexcept {
  return std::realloc(p, s);
}

void StdFree(void* p) noexcept {
  std::free(p);
}
}  // namespace

CSMemoryAllocator::CSMemoryAllocator() noexcept
: malloc_fn_(StdMalloc),
realloc_fn_(StdRealloc),
free_fn_(StdFree),
inited_(false) {
}

CSMemoryAllocator& CSMemoryAllocator::Instance() noexcept {
  static CSMemoryAllocator instance;
  return instance;
}

bool CSMemoryAllocator::SetAllocator(MallocFn malloc_fn,
    ReallocFn realloc_fn, FreeFn free_fn) noexcept {
  if (inited_) {
    return false;
  }
  malloc_fn_ = malloc_fn;
  realloc_fn_ = realloc_fn;
  free_fn_ = free_fn;
  inited_ = true;
  return true;
}

void* CSMemoryAllocator::Malloc(std::size_t size,
    std::uint32_t align) const noexcept {
  return malloc_fn_(size, align);
}

void* CSMemoryAllocator::Realloc(void* ptr,
    std::size_t size,
    std::uint32_t align) const noexcept {
  return realloc_fn_(ptr, size, align);
}

void CSMemoryAllocator::Free(void* ptr) const noexcept {
  free_fn_(ptr);
}

}  // namespace cornerstone
