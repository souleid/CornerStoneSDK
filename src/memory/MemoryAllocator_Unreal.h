// Copyright 2025 souleid
#ifndef MEMORY_MEMORYALLOCATOR_UNREAL_H_
#define MEMORY_MEMORYALLOCATOR_UNREAL_H_


#include "IMemoryAllocator.h"
#include "HAL/UnrealMemory.h"

namespace cornerstone {
class MemoryAllocator_Unreal : public IMemoryAllocator {
 public:
  void* Allocate(std::size_t size) override;
  void Deallocate(void* ptr) override;
};
}  // namespace cornerstone

#endif  // MEMORY_MEMORYALLOCATOR_UNREAL_H_
