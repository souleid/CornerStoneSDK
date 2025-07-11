// Copyright 2025 souleid
// PlatformServices_Default provides default platform service implementations.

#ifndef SRC_PLATFORM_PLATFORMSERVICES_DEFAULT_H_
#define SRC_PLATFORM_PLATFORMSERVICES_DEFAULT_H_

#include "src/platform/ISDKPlatformServices.h"
#include "src/memory/MemoryAllocator_Default.h"
#include "src/thread/ThreadAllocator_Default.h"

namespace cornerstone {

// Default implementation of platform services using std memory and threading.
class PlatformServices_Default : public ISDKPlatformServices {
 public:
  PlatformServices_Default();

  IMemoryAllocator* GetMemoryAllocator() override;
  IThreadAllocator* GetThreadAllocator() override;

  uint64_t GetCurrentTimeMs() override;
  void Log(const char* message) override;

 private:
  MemoryAllocator_Default memory_allocator_;
  ThreadAllocator_Default thread_allocator_;
};

}  // namespace cornerstone

#endif  // SRC_PLATFORM_PLATFORMSERVICES_DEFAULT_H_
