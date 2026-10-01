#pragma once

#include "ytdl/YtdlTypes.h"
#include <filesystem>
#include <string>
#include <vector>

namespace ytdlnis::ytdl {

struct RuntimePaths {
    std::filesystem::path ytDlp;
    std::filesystem::path ffmpeg;
    std::filesystem::path ffprobe;
};

class CommandBuilder {
public:
    static std::vector<std::wstring> build(const YtdlRequest& request,
                                           const RuntimePaths& runtime);
};

} // namespace ytdlnis::ytdl
