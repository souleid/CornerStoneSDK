// Copyright 2025 souleid
// IThread defines an abstract thread interface.

#ifndef SRC_THREAD_ITHREAD_H_
#define SRC_THREAD_ITHREAD_H_

namespace cornerstone {

// Abstract interface for platform-independent threads.
class IThread {
 public:
  virtual ~IThread() = default;

  // Starts execution of the thread.
  virtual void Start() = 0;

  // Blocks until the thread finishes execution.
  virtual void Join() = 0;
};

}  // namespace cornerstone

#endif  // SRC_THREAD_ITHREAD_H_
