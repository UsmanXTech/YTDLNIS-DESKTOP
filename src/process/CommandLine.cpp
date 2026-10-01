#include "ytdl/CommandLine.h"

namespace ytdlnis::process {

std::wstring quoteWindowsArgument(const std::wstring& argument) {
    if (argument.empty()) return L"\"\"";

    bool needsQuotes = false;
    for (wchar_t c : argument) {
        if (c == L' ' || c == L'\t' || c == L'\n' || c == L'\v' || c == L'\"') {
            needsQuotes = true;
            break;
        }
    }
    if (!needsQuotes) return argument;

    std::wstring result;
    result.push_back(L'\"');
    size_t backslashes = 0;
    for (wchar_t c : argument) {
        if (c == L'\\') {
            ++backslashes;
        } else if (c == L'\"') {
            result.append(backslashes * 2 + 1, L'\\');
            result.push_back(L'\"');
            backslashes = 0;
        } else {
            result.append(backslashes, L'\\');
            backslashes = 0;
            result.push_back(c);
        }
    }
    result.append(backslashes * 2, L'\\');
    result.push_back(L'\"');
    return result;
}

std::wstring buildWindowsCommandLine(const std::vector<std::wstring>& arguments) {
    std::wstring commandLine;
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i != 0) commandLine.push_back(L' ');
        commandLine += quoteWindowsArgument(arguments[i]);
    }
    return commandLine;
}

} // namespace ytdlnis::process
