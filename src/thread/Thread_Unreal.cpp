// Copyright 2025 souleid
namespace cornerstone {
#include "Thread_Unreal.h"
#include "HAL/PlatformProcess.h"

Thread_Unreal::Thread_Unreal(std::function<void()> task)
    : task_(std::move(task)),
      thread_(nullptr),
      running_(false),
      stop_requested_(false) {}

Thread_Unreal::~Thread_Unreal() {
  Join();
  if (thread_) {
    delete thread_;
    thread_ = nullptr;
  }
}

void Thread_Unreal::Start() {
  if (thread_ == nullptr) {
    thread_ = FRunnableThread::Create(
        this,
        TEXT("CornerStoneSDK_Thread"),
        0,                      // stack size (0 = default)
        TPri_Normal);          // priority
  }
}

void Thread_Unreal::Join() {
  if (thread_) {
    thread_->WaitForCompletion();
  }
}

bool Thread_Unreal::IsRunning() const {
  return running_;
}

bool Thread_Unreal::Init() {
  return true;
}

uint32 Thread_Unreal::Run() {
  running_ = true;
  if (task_) {
    task_();
  }
  running_ = false;
  return 0;
}

void Thread_Unreal::Stop() {
  stop_requested_ = true;
}

void Thread_Unreal::Exit() {
  // Optional: clean up here if needed
}
}  // namespace cornerstone
