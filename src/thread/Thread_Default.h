// Copyright 2025 souleid
// Thread_Default implements IThread using std::thread.

#ifndef SRC_THREAD_THREAD_DEFAULT_H_
#define SRC_THREAD_THREAD_DEFAULT_H_

#include <functional>
#include <memory>
#include <thread>

#include "src/thread/IThread.h"

namespace cornerstone {

// IThread implementation using std::thread for general C++ platforms.
class Thread_Default : public IThread {
 public:
  explicit Thread_Default(std::function<void()> entry_point);
  ~Thread_Default() override;

  void Start() override;
  void Join() override;

 private:
  std::function<void()> entry_point_;
  std::unique_ptr<std::thread> thread_;
};

}  // namespace cornerstone

#endif  // SRC_THREAD_THREAD_DEFAULT_H_
