// Copyright 2025 souleid
#ifndef SRC_MEMORY_MEMORYUTILS_H_
#define SRC_MEMORY_MEMORYUTILS_H_

#include <memory>
#include <utility>

#include "src/memory/csnewdelete.h"

namespace cornerstone {

template <class T>
struct CSDeleter {
  void operator()(T* p) const noexcept { CSDelete(p); }
};

namespace util {

template <class T, class... Args>
std::unique_ptr<T, CSDeleter<T>> MakeUnique(Args&&... args) {
  return std::unique_ptr<T, CSDeleter<T>>(
    CSNew<T>(static_cast<Args&&>(args)...));
}

template <class T, class... Args>
std::shared_ptr<T> MakeShared(Args&&... args) {
  return std::shared_ptr<T, CSDeleter<T>>(
    CSNew<T>(static_cast<Args&&>(args)...));
}
}  // namespace util
}  // namespace cornerstone

#endif  // SRC_MEMORY_MEMORYUTILS_H_
