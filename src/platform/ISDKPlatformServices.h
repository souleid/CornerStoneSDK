// Copyright 2025 souleid

#ifndef SRC_PLATFORM_ISDKPLATFORMSERVICES_H_
#define SRC_PLATFORM_ISDKPLATFORMSERVICES_H_

#include "src/memory/IMemoryAllocator.h"
#include "src/thread/IThreadAllocator.h"

namespace cornerstone {

// Provides platform-specific services used by the SDK.
class ISDKPlatformServices {
 public:
  virtual ~ISDKPlatformServices() = default;

  virtual IMemoryAllocator* GetMemoryAllocator() = 0;
  virtual IThreadAllocator* GetThreadAllocator() = 0;

  // Returns current monotonic time in milliseconds.
  virtual uint64_t GetCurrentTimeMs() = 0;

  // Logs a message to the platform's logging system.
  virtual void Log(const char* message) = 0;
};

}  // namespace cornerstone

#endif  // SRC_PLATFORM_ISDKPLATFORMSERVICES_H_
