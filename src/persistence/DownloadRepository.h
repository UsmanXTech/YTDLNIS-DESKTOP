#pragma once
#include "download/DownloadJob.h"
#include "persistence/Database.h"
#include <cstdint>
#include <string>
#include <vector>
namespace ytdlnis::persistence {
struct PersistedDownload {
    std::uint64_t id=0;
    std::string url;
    download::JobState state=download::JobState::Queued;
    double percent=-1.0;
    std::string speed;
    std::string eta;
    std::string error;
    std::int64_t createdAt=0;
    std::int64_t updatedAt=0;
};
class DownloadRepository {
public:
    explicit DownloadRepository(Database& database) : database_(database) {}
    bool upsert(const PersistedDownload& download, std::wstring& error);
    bool remove(std::uint64_t id, std::wstring& error);
    std::vector<PersistedDownload> all(std::wstring& error);
private:
    Database& database_;
};
}