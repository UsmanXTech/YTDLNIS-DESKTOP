#pragma once
#include "ytdl/YtdlEngine.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
namespace ytdlnis::download {
enum class JobState { Queued, Starting, Downloading, Completed, Failed, Cancelled };
struct DownloadJobSnapshot { std::uint64_t id=0; JobState state=JobState::Queued; double percent=-1.0; std::wstring speed; std::wstring eta; std::wstring error; };
class DownloadJob {
public:
 using UpdateCallback=std::function<void(const DownloadJobSnapshot&)>;
 DownloadJob(std::uint64_t id, ytdl::YtdlRequest request, ytdl::runtime::RuntimeManager& runtime, UpdateCallback callback={});
 ~DownloadJob();
 DownloadJob(const DownloadJob&)=delete;
 DownloadJob& operator=(const DownloadJob&)=delete;
 void start();
 void cancel();
 DownloadJobSnapshot snapshot() const;
private:
 std::uint64_t id_; ytdl::YtdlRequest request_; ytdl::runtime::RuntimeManager& runtime_; UpdateCallback callback_;
 mutable std::mutex mutex_; DownloadJobSnapshot snapshot_; std::unique_ptr<ytdl::YtdlEngine> engine_; std::thread worker_;
};
}