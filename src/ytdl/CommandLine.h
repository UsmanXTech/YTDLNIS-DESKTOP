#pragma once

#include <string>
#include <vector>

namespace ytdlnis::process {

// Converts a Windows argv vector into a CreateProcessW-compatible command line.
// The executable path remains a separate value in the process API; this function
// only performs Windows argument quoting and does not invoke a shell.
std::wstring quoteWindowsArgument(const std::wstring& argument);
std::wstring buildWindowsCommandLine(const std::vector<std::wstring>& arguments);

} // namespace ytdlnis::process
