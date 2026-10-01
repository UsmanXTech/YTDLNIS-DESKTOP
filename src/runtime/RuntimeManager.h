#pragma once
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
namespace ytdlnis::runtime {
enum class ToolKind { YtDlp, Ffmpeg, Ffprobe, Aria2c, Deno, Node, QuickJs };
struct ToolInfo { ToolKind kind; std::filesystem::path path; std::wstring version; bool available=false; bool executable=false; };
struct RuntimeLayout {
 std::filesystem::path root, runtime, ytDlp, ffmpeg, aria2, node, deno, quickJs;
 std::filesystem::path data, cache, logs, cookies, templates;
};
class RuntimeManager {
public:
 static RuntimeLayout defaultLayout();
 static RuntimeLayout resolveLayout(const std::optional<std::filesystem::path>& root);
 explicit RuntimeManager(RuntimeLayout layout);
 [[nodiscard]] const RuntimeLayout& layout() const noexcept;
 ToolInfo discover(ToolKind kind) const;
 std::vector<ToolInfo> diagnostics() const;
private: RuntimeLayout layout_;
};
} // namespace ytdlnis::runtime
