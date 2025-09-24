// Copyright 2025 souleid
#ifndef SRC_MEMORY_CSNEWDELETE_H_
#define SRC_MEMORY_CSNEWDELETE_H_

#include <new>
#include <utility>
#include "src/memory/csmemoryallocator.h"

namespace cornerstone {

template <class T, class... Args>
T* CSNew(Args&&... args) {
  void* mem = CSMemoryAllocator::Instance()
    .Malloc(sizeof(T), alignof(T));
  if (!mem) throw std::bad_alloc();
  try {
    return ::new (mem) T(static_cast<Args&&>(args)...);
  }
  catch (...) {
    CSMemoryAllocator::Instance().Free(mem);
    throw;
  }
}

template <class T>
void CSDelete(T* p) noexcept {
  if (!p) return;
  p->~T();
  CSMemoryAllocator::Instance().Free(static_cast<void*>(p));
}

}  // namespace cornerstone

#endif  // SRC_MEMORY_CSNEWDELETE_H_
