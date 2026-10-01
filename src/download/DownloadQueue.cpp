#include "download/DownloadQueue.h"

#include <algorithm>

namespace ytdlnis::download {

DownloadQueue::DownloadQueue(ytdl::runtime::RuntimeManager& runtime, std::size_t concurrency)
    : runtime_(runtime), concurrency_(std::max<std::size_t>(1, concurrency)) {}

std::uint64_t DownloadQueue::enqueue(ytdl::YtdlRequest request, DownloadJob::UpdateCallback callback) {
    std::lock_guard lock(mutex_);
    const auto id = nextId_++;
    jobs_.push_back(std::make_unique<DownloadJob>(id, std::move(request), runtime_, std::move(callback)));
    pump();
    return id;
}

void DownloadQueue::cancel(std::uint64_t id) {
    std::lock_guard lock(mutex_);
    for (auto& job : jobs_) {
        if (job->snapshot().id == id) {
            job->cancel();
            break;
        }
    }
    pump();
}

std::vector<DownloadJobSnapshot> DownloadQueue::snapshots() const {
    std::lock_guard lock(mutex_);
    std::vector<DownloadJobSnapshot> result;
    result.reserve(jobs_.size());
    for (const auto& job : jobs_) result.push_back(job->snapshot());
    return result;
}

void DownloadQueue::pump() {
    std::size_t active = 0;
    for (const auto& job : jobs_) {
        const auto state = job->snapshot().state;
        if (state == JobState::Starting || state == JobState::Downloading) ++active;
    }
    for (auto& job : jobs_) {
        if (active >= concurrency_) break;
        if (job->snapshot().state == JobState::Queued) {
            job->start();
            ++active;
        }
    }
}

} // namespace ytdlnis::download
