// Copyright 2025 souleid

#ifndef UNITY_CORNERSTONE_C_BRIDGE_HANDLE_H_
#define UNITY_CORNERSTONE_C_BRIDGE_HANDLE_H_

#include <cstddef>
#include <cstdint>

#if defined(_WIN32)
#define CS_API __declspec(dllexport)
#define CS_CALL __cdecl
#else
#define CS_API __attribute__((visibility("default")))
#define CS_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif
  typedef int CSResult;

  typedef void* CSHandle;

  typedef void(CS_CALL* CSWorker)(void* work);

  typedef void (*CSResultCallback)(std::uint64_t id, CSResult err, void* user);

  // Create/destroy a core instance.
  CS_API CSResult CS_CALL CSCreate(std::uint32_t num_threads,
                           CSHandle* out);

  CS_API CSResult CS_CALL CSDestory(CSHandle h);

  CS_API CSResult CS_CALL CSRequest(CSHandle h, CSWorker work,
                            void* work_arg,
                            CSResultCallback cb, void* cb_arg,
                            std::uint64_t* out_id);

  CS_API CSResult CS_CALL CSCancel(CSHandle h, std::uint64_t id);
  CS_API CSResult CS_CALL CSCancelAll(CSHandle h);

  CS_API const char* CS_CALL CSLastError(void);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // UNITY_CORNERSTONE_C_BRIDGE_HANDLE_H_