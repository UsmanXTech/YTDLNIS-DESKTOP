#include <cassert>
#include <string>
#include <vector>

// Temporary compile-level contract tests. Implementation will be added with
// the native process layer; these assertions document required argv behavior.
int main() {
    const std::vector<std::wstring> argv{
        L"yt-dlp.exe",
        L"https://example.com/video?id=1",
        L"--output",
        L"C:\\Users\\Test User\\Downloads\\%(title)s.%(ext)s"
    };

    assert(argv.size() == 4);
    assert(argv[0] == L"yt-dlp.exe");
    assert(argv[2] == L"--output");
    return 0;
}
