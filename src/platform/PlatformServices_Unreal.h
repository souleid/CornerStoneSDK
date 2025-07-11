// Copyright 2025 souleid
#ifndef PLATFORM_PLATFORMSERVICES_UNREAL_H_
#define PLATFORM_PLATFORMSERVICES_UNREAL_H_


#include "ISDKPlatformServices.h"
#include "ThreadAllocator_Unreal.h"
#include "MemoryAllocator_Unreal.h"

namespace cornerstone {

class PlatformServices_Unreal : public ISDKPlatformServices {
 public:
  PlatformServices_Unreal();
  ~PlatformServices_Unreal() override;

  IThreadAllocator* GetThreadAllocator() override;
  IMemoryAllocator* GetMemoryAllocator() override;

 private:
  ThreadAllocator_Unreal thread_allocator_;
  MemoryAllocator_Unreal memory_allocator_;
};
}  // namespace cornerstone

#endif  // PLATFORM_PLATFORMSERVICES_UNREAL_H_
