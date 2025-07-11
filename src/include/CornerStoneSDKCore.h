// Copyright 2025 souleid
// CornerStoneSDKCore is the top-level interface for using the SDK.

#ifndef SRC_INCLUDE_CORNERSTONESDKCORE_H_
#define SRC_INCLUDE_CORNERSTONESDKCORE_H_

#include <utility>

#include "src/platform/ISDKPlatformServices.h"
#include "src/thread/ManagedThreadPool.h"

namespace cornerstone {

// Core SDK interface, provides async task execution using platform services.
class CornerStoneSDKCore {
 public:
  explicit CornerStoneSDKCore(ISDKPlatformServices* platform_services);
  ~CornerStoneSDKCore();

  void Start(std::size_t thread_count = 4);
  void Shutdown();

  template <typename Func, typename ResultCallback>
  TaskId EnqueueAsync(Func func, ResultCallback callback);
  bool CancelTask(TaskId id);

 private:
  ISDKPlatformServices* platform_;
  ManagedThreadPool* thread_pool_;
};

// Template implementation.
template <typename Func, typename ResultCallback>
TaskId CornerStoneSDKCore::EnqueueAsync(Func func, ResultCallback callback) {
  if (thread_pool_) {
    return thread_pool_->EnqueueWithResult(
        std::move(func), std::move(callback));
  }
  return 0;
}

}  // namespace cornerstone

#endif  // SRC_INCLUDE_CORNERSTONESDKCORE_H_
