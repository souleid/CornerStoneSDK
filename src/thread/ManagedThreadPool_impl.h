// Copyright 2025 souleid
// Template implementation of EnqueueWithResult.

#ifndef SRC_THREAD_MANAGEDTHREADPOOL_IMPL_H_
#define SRC_THREAD_MANAGEDTHREADPOOL_IMPL_H_

#include <cstdio>
#include <type_traits>
#include <utility>

namespace cornerstone {

namespace internal {

template <typename Func, typename ResultCallback>
void InvokeWithResult(Func func, ResultCallback callback, std::false_type) {
  auto result = func();
  callback(result);
}

template <typename Func, typename ResultCallback>
void InvokeWithResult(Func func, ResultCallback callback, std::true_type) {
  func();
  callback();  // callback takes no argument
}

}  // namespace internal

template <typename Func, typename ResultCallback>
TaskId ManagedThreadPool::EnqueueWithResult(
    Func task_func, ResultCallback callback) {
  TaskId id = next_task_id_.fetch_add(1);

  auto wrapper = [task_func, callback]() {
    using is_void = typename std::is_void<decltype(task_func())>::type;
    internal::InvokeWithResult(task_func, callback, is_void());
    };

  EnqueueWithId(id, std::move(wrapper));

  return id;
}

}  // namespace cornerstone

#endif  // SRC_THREAD_MANAGEDTHREADPOOL_IMPL_H_
