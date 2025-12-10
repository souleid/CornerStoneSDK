// Copyright 2025 souleid
#include "src/task/task_download.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

namespace cornerstone {

struct CallbackState {
  std::ofstream* file_stream;
  TaskDownload* task;
};

TaskDownload::TaskDownload(uint64_t task_id,
  std::string url,
  std::string target_path,
  ProgressCallback prog_cb)
  : Task(task_id),
  url_(std::move(url)),
  target_path_(std::move(target_path)),
  progress_cb_(std::move(prog_cb)) {
}

std::size_t TaskDownload::WriteCallback(void* ptr, std::size_t size,
  std::size_t nmemb, void* stream) {
  CallbackState* state = static_cast<CallbackState*>(stream);
  std::size_t total_size = size * nmemb;
  state->file_stream->write(static_cast<const char*>(ptr), total_size);
  return total_size;
}

int TaskDownload::XferinfoCallback(void* clientp, curl_off_t dltotal,
  curl_off_t dlnow, curl_off_t ultotal,
  curl_off_t ulnow) {
  CallbackState* state = static_cast<CallbackState*>(clientp);
  TaskDownload* task = state->task;

  if (task->IsCancelled()) {
    return 1;
  }

  if (dltotal > 0 && dlnow <= dltotal) {
    double progress = static_cast<double>(dlnow) / static_cast<double>(dltotal);
    if (task->progress_cb_) {
      task->progress_cb_(task->GetTaskId(), progress);
    }
  }

  return 0;
}

int TaskDownload::Run() {
  int result_code = static_cast<int>(TaskResultCode::kGenericError);

  CURL* curl = curl_easy_init();
  if (!curl) return result_code;

  std::ofstream outfile(target_path_, std::ios::binary);
  if (!outfile.is_open()) {
    curl_easy_cleanup(curl);
    return static_cast<int>(TaskResultCode::kFileError);
  }

  CallbackState state = { &outfile, this };

  curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &state);

  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, XferinfoCallback);
  curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &state);

  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

  CURLcode res = curl_easy_perform(curl);

  outfile.close();
  curl_easy_cleanup(curl);

  if (res == CURLE_OK) {
    if (IsCancelled()) {
      result_code = static_cast<int>(TaskResultCode::kCancelled);
    } else {
      result_code = static_cast<int>(TaskResultCode::kSuccess);
    }
  } else {
    result_code = static_cast<int>(TaskResultCode::kNetworkError);
  }

  return result_code;
}

}  // namespace cornerstone
