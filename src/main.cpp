// Copyright 2025 souleid
#include <chrono>
#include <cstdio>
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <utility>

#include "src/cornerstonesdkcore.h"
#include "src/task/task_download.h"

const std::vector<std::pair<std::string, std::string>> kTestDownloads = {
    {"https://sample-files.com/downloads/documents/txt/simple.txt", "temp_file1.dat"}, // NOLINT
    {"https://sample-files.com/downloads/documents/txt/long-doc.txt", "temp_file2.dat"}, // NOLINT
    {"https://sample-files.com/downloads/documents/txt/ascii-art.txt", "temp_file3.dat"} // NOLINT
};

std::atomic<int> g_completed_tasks{0};

void RunDownloadTest(cornerstone::CornerstoneSdkCore& sdk) {
  std::vector<uint64_t> ids;
  int total_tasks = kTestDownloads.size();

  auto result_handler = [&](uint64_t id, int result_code) {
    std::printf("[TASK %llu][FINAL] Result Code: %d (%s)\n",
      static_cast<uint64_t>(id), result_code,
      (result_code == static_cast<int>(cornerstone::TaskResultCode::kSuccess)
      ? "SUCCESS" : "FAIL/CANCELED"));
    g_completed_tasks++;
    };

  auto progress_handler = [&](uint64_t id, double progress) {
    std::printf("[TASK %llu][PROGRESS] %.2f%%\n",
      static_cast<uint64_t>(id), progress * 100.0);
    };

  uint64_t current_id_counter = 100;
  for (size_t i = 0; i < kTestDownloads.size(); ++i) {
    const auto& item = kTestDownloads[i];

    auto task = cornerstone::util::MakeUnique<cornerstone::TaskDownload>(
        current_id_counter++,
        item.first,
        item.second,
        progress_handler);

    uint64_t id = sdk.RequestTask(std::move(task), result_handler);
    ids.push_back(id);
    std::printf("[MAIN] Submitted Task ID: %llu\n", static_cast<uint64_t>(id));
  }

  auto start_time = std::chrono::steady_clock::now();
  while (g_completed_tasks.load() < total_tasks) {
    if (std::chrono::steady_clock::now() -
        start_time > std::chrono::seconds(5)) {
      std::printf("[MAIN] TIMEOUT: Downloads did not finish within 5s. Forcing shutdown.\n");  // NOLINT
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

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

  cornerstone::TaskId cancel_id = sdk.Request([]() {
    std::printf("[cancel-me] should not run\n");
    });
  bool canceled = sdk.CancelById(cancel_id);
  std::printf("[cancel] id=%llu done=%s\n",
    static_cast<uint64_t>(cancel_id),
    canceled ? "true" : "false");

  RunDownloadTest(sdk);

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  sdk.Shutdown();
  std::printf("[SDK] shutdown\n");
  return 0;
}
