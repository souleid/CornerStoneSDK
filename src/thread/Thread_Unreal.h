// Copyright 2025 souleid
namespace cornerstone {
#ifndef THREADING_THREAD_UNREAL_H_
#define THREADING_THREAD_UNREAL_H_


#include "IThread.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include <atomic>
#include <functional>

class Thread_Unreal : public IThread, public FRunnable {
 public:
  explicit Thread_Unreal(std::function<void()> task);
  ~Thread_Unreal() override;

  void Start() override;
  void Join() override;
  bool IsRunning() const override;

  // FRunnable overrides
  bool Init() override;
  uint32 Run() override;
  void Stop() override;
  void Exit() override;

 private:
  std::function<void()> task_;
  FRunnableThread* thread_;
  std::atomic<bool> running_;
  std::atomic<bool> stop_requested_;
};
}  // namespace cornerstone

#endif  // THREADING_THREAD_UNREAL_H_
