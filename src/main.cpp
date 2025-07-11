// Copyright 2025 souleid
#include <cstdio>
#include <string>
#include <thread>
#include <chrono>

#include "src/include/CornerStoneSDKCore.h"
#include "src/platform/PlatformServices_Default.h"

int main() {
  cornerstone::PlatformServices_Default platform;
  cornerstone::CornerStoneSDKCore sdk(&platform);

  sdk.Start(6);  // Start with 6 worker threads

  // Task 1: int computation
  sdk.EnqueueAsync(
      []() -> int {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return 42;
      },
      [](int result) {
        std::printf("[Callback] Result1: %d\n", result);
      });

  // Task 2: string return
  auto id_ok = sdk.EnqueueAsync(
    []() -> std::string {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      return "Hello from async!";
    },
    [](const std::string& result) {
      std::printf("[Callback] Received: %s\n", result.c_str());
    });

  // Task 3: float computation
  sdk.EnqueueAsync(
      []() -> float {
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
        return 3.1415f;
      },
      [](float result) {
        std::printf("[Callback] Result3: %.4f\n", result);
      });

  // Task 4: heavy int
  sdk.EnqueueAsync(
      []() -> int {
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        return 999;
      },
      [](int result) {
        std::printf("[Callback] Result4: %d\n", result);
      });

  // Task 5: void return
  sdk.EnqueueAsync(
      []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::printf("[Task] Void task done.\n");
        return;
      },
      []() {
        std::printf("[Callback] Void return emulated.\n");
      });

  // Task 6: multiple thread ID log
  sdk.EnqueueAsync(
      []() -> std::string {
        std::this_thread::sleep_for(std::chrono::milliseconds(70));
        return "Thread ID Task";
      },
      [](const std::string& result) {
        std::printf("[Callback] Result6: %s\n", result.c_str());
      });

  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  sdk.Shutdown();

  return 0;
}
