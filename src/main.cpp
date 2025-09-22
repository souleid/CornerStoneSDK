// Copyright 2025 souleid
#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>

#include "src/cornerstonesdkcore.h"

int main() {
  cornerstone::CornerstoneSdkCore sdk;
  if (!sdk.Init(4)) {
    std::printf("[SDK] init failed\n");
    return 1;
  }
  std::printf("[SDK] init ok\n");

  std::vector<cornerstone::TaskId> ids;
  for (int i = 0; i < 5; ++i) {
    cornerstone::TaskId id = sdk.Request([i]() {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      std::printf("[task %d] work done\n", i);
      });
    ids.push_back(id);
  }

  cornerstone::TaskId rid = sdk.RequestWithResult(
    []() -> int {
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
      return 42;
    },
    [](int v) {
      std::printf("[result] value=%d\n", v);
    });
  ids.push_back(rid);

  // Demo: enqueue then cancel one task before it runs.
  cornerstone::TaskId cancel_id = sdk.Request([]() {
    std::printf("[cancel-me] should not run\n");
    });
  bool canceled = sdk.CancelById(cancel_id);
  std::printf("[cancel] id=%llu done=%s\n",
    static_cast<uint64_t>(cancel_id),
    canceled ? "true" : "false");

  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  sdk.Shutdown();
  std::printf("[SDK] shutdown\n");
  return 0;
}
