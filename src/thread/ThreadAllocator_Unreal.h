// Copyright 2025 souleid
namespace cornerstone {
#ifndef THREADING_THREADALLOCATOR_UNREAL_H_
#define THREADING_THREADALLOCATOR_UNREAL_H_


#include <memory>
#include <functional>

#include "IThreadAllocator.h"
#include "IMemoryAllocator.h"
#include "Thread_Unreal.h"
#include "MemoryUtils.h"  // cs::MakeUnique

class ThreadAllocator_Unreal : public IThreadAllocator {
 public:
  explicit ThreadAllocator_Unreal(IMemoryAllocator* memory_allocator);

  std::unique_ptr<IThread> CreateThread(
      std::function<void()> entry) override;

 private:
  IMemoryAllocator* memory_allocator_;
};
}  // namespace cornerstone

#endif  // THREADING_THREADALLOCATOR_UNREAL_H_
