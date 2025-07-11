// Copyright 2025 souleid
#ifndef SRC_MEMORY_MEMORYUTILS_H_
#define SRC_MEMORY_MEMORYUTILS_H_


#include <memory>
#include <functional>
#include <utility>

#include "src/memory/IMemoryAllocator.h"

namespace cornerstone {

// Creates an allocator-aware unique_ptr using the given IMemoryAllocator.
// The returned unique_ptr uses a custom deleter that calls destructor and
// allocator->Deallocate().
template <typename T, typename... Args>
std::unique_ptr<T, std::function<void(T*)>> MakeUnique(
    IMemoryAllocator* allocator, Args&&... args) {
  void* raw = allocator->Allocate(sizeof(T));
  T* obj = new (raw) T(std::forward<Args>(args)...);
  return std::unique_ptr<T, std::function<void(T*)>>(
      obj,
      [allocator](T* ptr) {
        if (ptr) {
          ptr->~T();
          allocator->Deallocate(ptr);
        }
      });
}
}  // namespace cornerstone

#endif  // SRC_MEMORY_MEMORYUTILS_H_
