#pragma once

#include <windows.h>

#include <functional>
#include <string>
#include <vector>

namespace ytdlnis::process {

struct ProcessOutput {
    std::wstring stdoutText;
    std::wstring stderrText;
    DWORD exitCode = STILL_ACTIVE;
};

struct ProcessSpec {
    std::wstring executable;
    std::vector<std::wstring> arguments;
    std::wstring workingDirectory;
    bool hideWindow = true;
};

class ProcessController {
public:
    using OutputCallback = std::function<void(bool isStdErr, const std::wstring& text)>;

    ProcessController() = default;
    ~ProcessController();

    ProcessController(const ProcessController&) = delete;
    ProcessController& operator=(const ProcessController&) = delete;

    bool start(const ProcessSpec& spec, OutputCallback callback = {});
    bool terminate(UINT exitCode = 1);
    bool wait(DWORD timeoutMs = INFINITE);

    [[nodiscard]] bool running() const noexcept;
    [[nodiscard]] DWORD processId() const noexcept;
    [[nodiscard]] DWORD exitCode() const noexcept;

private:
    struct State;
    State* state_ = nullptr;
};

} // namespace ytdlnis::process
