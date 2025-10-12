// Copyright 2025 souleid
// C bridge for CornerstoneSdkCore without a per-instance wrapper.
// The CSHandle is treated as a CornerstoneSdkCore* directly.

#include "cornerstone_c_api.h"

#include <cstddef>
#include <cstdint>
#include <new>

#include "src/cornerstonesdkcore.h"

namespace {

  // Thread-local last error string. Empty means no error.
  thread_local const char* t_last_err = "";

  // Helper to set thread-local error text.
  inline void SetErr(const char* msg) { t_last_err = msg ? msg : ""; }

  // Validate handle and return typed pointer or set error.
  inline cornerstone::CornerstoneSdkCore* FromHandle(CSHandle h) {
    if (!h) {
      SetErr("invalid handle");
      return nullptr;
    }
    return reinterpret_cast<cornerstone::CornerstoneSdkCore*>(h);
  }

}  // namespace

extern "C" {

  CS_API CSResult CSCreate(std::uint32_t num_threads, CSHandle* out) {
    if (!out) {
      SetErr("invalid arguments");
      return -1;
    }

    cornerstone::CornerstoneSdkCore* core = nullptr;
    try {
      core = new cornerstone::CornerstoneSdkCore();
    }
    catch (...) {
      SetErr("core construction failed");
      return -2;
    }

    const std::size_t n = static_cast<std::size_t>(num_threads);
    const bool ok = core->Init(n);
    if (!ok) {
      delete core;
      SetErr("core init failed");
      return -3;
    }

    *out = reinterpret_cast<CSHandle>(core);
    return 0;
  }

  // Note: API name follows the header's 'Destory' spelling.
  CS_API CSResult CSDestory(CSHandle h) {
    auto* core = FromHandle(h);
    if (!core) return -1;

    core->Shutdown();
    delete core;
    return 0;
  }

  CS_API CSResult CSRequest(CSHandle h,
    CSWorker work,
    void* work_arg,
    CSResultCallback cb,
    void* cb_arg,
    std::uint64_t* out_id) {
    if (!work || !cb || !out_id) {
      SetErr("invalid arguments");
      return -1;
    }
    auto* core = FromHandle(h);
    if (!core) return -1;
    if (!core->IsRunning()) {
      SetErr("core not running");
      return -2;
    }

    // Capture id by reference into the completion callback.
    std::uint64_t captured_id = 0;

    captured_id = core->RequestWithResult(
      [work, work_arg]() -> int {
        work(work_arg);
        return 0; // return error code (0 = success) 
      },
      [cb, cb_arg, &captured_id](int err_code) {
        cb(captured_id, static_cast<CSResult>(err_code), cb_arg);
      });

    if (captured_id == 0) {
      SetErr("enqueue failed");
      return -3;
    }

    *out_id = captured_id;
    return 0;
  }

  CS_API CSResult CSCancel(CSHandle h, std::uint64_t id) {
    auto* core = FromHandle(h);
    if (!core) return -1;
    core->CancelById(id);
    return 0;
  }

  CS_API CSResult CSCancelAll(CSHandle h) {
    auto* core = FromHandle(h);
    if (!core) return -1;
    core->CancelAll();
    return 0;
  }

  CS_API const char* CSLastError(void) {
    return t_last_err ? t_last_err : "";
  }

}  // extern "C"
