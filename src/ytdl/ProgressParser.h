#pragma once
#include "ytdl/YtdlEngine.h"
#include <string>
namespace ytdlnis::ytdl { class ProgressParser { public: static ProgressEvent parse(const std::wstring& line); }; }