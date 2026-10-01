#pragma once
#include "runtime/RuntimeManager.h"
#include <string>
namespace ytdlnis::runtime {
struct VersionResult { bool success=false; std::wstring version; std::wstring error; };
VersionResult detectVersion(const ToolInfo& tool);
}
