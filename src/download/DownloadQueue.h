#pragma once
#include "download/DownloadJob.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>
namespace ytdlnis::download {
class DownloadQueue {
public:
 explicit DownloadQueue(ytdl::runtime::RuntimeManager& runtime, std::size_t concurrency=2);
 std::uint64_t enqueue(ytdl::YtdlRequest request, DownloadJob::UpdateCallback callback={});
 void cancel(std::uint64_t id);
 std::vector<DownloadJobSnapshot> snapshots() const;
private:
 void pump();
 void onJobComplete(std::uint64_t id);
 ytdl::runtime::RuntimeManager& runtime_;
 std::size_t concurrency_;
 std::uint64_t nextId_=1;
 mutable std::mutex mutex_;
 std::vector<std::unique_ptr<DownloadJob>> jobs_;
};
}