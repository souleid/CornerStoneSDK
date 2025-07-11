// Copyright 2025 souleid
namespace cornerstone {
#include "ThreadAllocator_Unreal.h"

ThreadAllocator_Unreal::ThreadAllocator_Unreal(
    IMemoryAllocator* memory_allocator)
    : memory_allocator_(memory_allocator) {}

std::unique_ptr<IThread> ThreadAllocator_Unreal::CreateThread(
    std::function<void()> entry) {
  return cs::MakeUnique<Thread_Unreal>(memory_allocator_, std::move(entry));
}
}  // namespace cornerstone
