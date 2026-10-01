#include "download/DownloadJob.h"
namespace ytdlnis::download {
DownloadJob::DownloadJob(std::uint64_t id, ytdl::YtdlRequest request, ytdl::runtime::RuntimeManager& runtime, UpdateCallback callback)
 : id_(id), request_(std::move(request)), runtime_(runtime), callback_(std::move(callback)) { snapshot_.id=id_; }
DownloadJob::~DownloadJob(){ cancel(); if(worker_.joinable()) worker_.join(); }
void DownloadJob::publish(){ if(callback_) callback_(snapshot()); }
void DownloadJob::start(){
 std::lock_guard lock(mutex_); if(worker_.joinable()) return; snapshot_.state=JobState::Starting; publish();
 worker_=std::thread([this]{
  { std::lock_guard lock(mutex_); snapshot_.state=JobState::Downloading; publish(); }
  engine_=std::make_unique<ytdl::YtdlEngine>(runtime_);
  auto result=engine_->start(request_,[this](const ytdl::ProgressEvent& e){ std::lock_guard lock(mutex_); snapshot_.percent=e.percent; snapshot_.speed=e.speed; snapshot_.eta=e.eta; publish(); });
  std::lock_guard lock(mutex_);
  if(snapshot_.state==JobState::Cancelled) return;
  snapshot_.state=result.completed?JobState::Completed:JobState::Failed; snapshot_.error=result.error; publish();
 });
}
void DownloadJob::cancel(){ std::lock_guard lock(mutex_); if(engine_) engine_->cancel(); if(snapshot_.state==JobState::Queued||snapshot_.state==JobState::Starting||snapshot_.state==JobState::Downloading){snapshot_.state=JobState::Cancelled; publish();} }
DownloadJobSnapshot DownloadJob::snapshot() const { std::lock_guard lock(mutex_); return snapshot_; }
}