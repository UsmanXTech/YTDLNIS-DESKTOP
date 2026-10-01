#include "download/DownloadJob.h"

namespace ytdlnis::download {

DownloadJob::DownloadJob(std::uint64_t id, ytdl::YtdlRequest request,
                         ytdl::runtime::RuntimeManager& runtime,
                         UpdateCallback callback, CompletionCallback completion)
    : id_(id), request_(std::move(request)), runtime_(runtime),
      callback_(std::move(callback)), completion_(std::move(completion)) {
    snapshot_.id = id_;
}

DownloadJob::~DownloadJob() {
    cancel();
    if (worker_.joinable()) worker_.join();
}

void DownloadJob::publish() {
    if (callback_) callback_(snapshot_);
}

void DownloadJob::start() {
    {
        std::lock_guard lock(mutex_);
        if (worker_.joinable() || snapshot_.state != JobState::Queued) return;
        snapshot_.state = JobState::Starting;
        publish();
    }

    worker_ = std::thread([this] {
        {
            std::lock_guard lock(mutex_);
            if (snapshot_.state == JobState::Cancelled) return;
            snapshot_.state = JobState::Downloading;
            publish();
        }

        engine_ = std::make_unique<ytdl::YtdlEngine>(runtime_);
        const auto result = engine_->start(
            request_,
            [this](const ytdl::ProgressEvent& event) {
                std::lock_guard lock(mutex_);
                if (snapshot_.state == JobState::Cancelled) return;
                snapshot_.percent = event.percent;
                snapshot_.speed = event.speed;
                snapshot_.eta = event.eta;
                publish();
            },
            [this](bool isStdErr, const std::wstring& text) {
                if (!isStdErr) return;
                std::lock_guard lock(mutex_);
                if (snapshot_.state != JobState::Cancelled && !text.empty()) snapshot_.error = text;
            });

        CompletionCallback completion;
        {
            std::lock_guard lock(mutex_);
            if (snapshot_.state == JobState::Cancelled) return;
            snapshot_.state = result.completed ? JobState::Completed : JobState::Failed;
            if (!result.error.empty()) snapshot_.error = result.error;
            publish();
            completion = completion_;
        }
        if (completion) completion(id_);
    });
}

void DownloadJob::cancel() {
    std::lock_guard lock(mutex_);
    if (snapshot_.state == JobState::Completed || snapshot_.state == JobState::Failed || snapshot_.state == JobState::Cancelled) return;
    snapshot_.state = JobState::Cancelled;
    if (engine_) engine_->cancel();
    publish();
}

DownloadJobSnapshot DownloadJob::snapshot() const {
    std::lock_guard lock(mutex_);
    return snapshot_;
}

} // namespace ytdlnis::download
