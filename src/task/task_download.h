// Copyright 2025 souleid
#ifndef SRC_TASK_TASK_DOWNLOAD_H_
#define SRC_TASK_TASK_DOWNLOAD_H_

#include <curl/curl.h>
#include <functional>
#include <string>

#include "src/task/task.h"

namespace cornerstone {

using ProgressCallback = std::function<void(uint64_t task_id, double progress)>;

class TaskDownload : public Task {
 public:
  TaskDownload(uint64_t task_id,
     std::string url,
     std::string target_path,
     ProgressCallback prog_cb);
  virtual ~TaskDownload() = default;

  int Run();

  static std::size_t WriteCallback(void* ptr, std::size_t size,
    std::size_t nmemb, void* stream);

  static int XferinfoCallback(void* clientp, curl_off_t dltotal,
    curl_off_t dlnow, curl_off_t ultotal,
    curl_off_t ulnow);

 private:
  std::string url_;
  std::string target_path_;
  ProgressCallback progress_cb_;
};

}  // namespace cornerstone

#endif  // SRC_TASK_TASK_DOWNLOAD_H_
