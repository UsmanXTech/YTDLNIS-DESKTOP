#include "process/ProcessController.h"
#include "ytdl/CommandLine.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <utility>

namespace ytdlnis::process {

namespace {
void closeHandle(HANDLE& handle) noexcept {
    if (handle && handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
    handle = nullptr;
}

bool createPipe(HANDLE& readHandle, HANDLE& writeHandle) {
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    if (!CreatePipe(&readHandle, &writeHandle, &sa, 0)) return false;
    if (!SetHandleInformation(readHandle, HANDLE_FLAG_INHERIT, 0)) {
        closeHandle(readHandle); closeHandle(writeHandle); return false;
    }
    return true;
}

std::wstring decodeChunk(const char* data, DWORD size) {
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, data, static_cast<int>(size), nullptr, 0);
    UINT cp = CP_UTF8;
    DWORD flags = MB_ERR_INVALID_CHARS;
    if (n <= 0) { cp = CP_ACP; flags = 0; n = MultiByteToWideChar(cp, flags, data, static_cast<int>(size), nullptr, 0); }
    if (n <= 0) return {};
    std::wstring out(static_cast<size_t>(n), L'\0');
    MultiByteToWideChar(cp, flags, data, static_cast<int>(size), out.data(), n);
    return out;
}
}

struct ProcessController::State {
    HANDLE process = nullptr;
    HANDLE thread = nullptr;
    HANDLE stdoutRead = nullptr;
    HANDLE stderrRead = nullptr;
    std::thread stdoutReader;
    std::thread stderrReader;
    std::mutex mutex;
    std::atomic<bool> running{false};
    DWORD pid = 0;
    DWORD exitCode = STILL_ACTIVE;
    OutputCallback callback;
};

ProcessController::~ProcessController() {
    terminate();
    if (!state_) return;
    if (state_->process) WaitForSingleObject(state_->process, INFINITE);
    if (state_->stdoutReader.joinable()) state_->stdoutReader.join();
    if (state_->stderrReader.joinable()) state_->stderrReader.join();
    closeHandle(state_->thread);
    closeHandle(state_->process);
    closeHandle(state_->stdoutRead);
    closeHandle(state_->stderrRead);
    delete state_;
    state_ = nullptr;
}

bool ProcessController::start(const ProcessSpec& spec, OutputCallback callback) {
    if (state_ && state_->running.load()) return false;
    if (!state_) state_ = new State();
    state_->callback = std::move(callback);

    HANDLE stdoutWrite = nullptr, stderrWrite = nullptr;
    if (!createPipe(state_->stdoutRead, stdoutWrite)) return false;
    if (!createPipe(state_->stderrRead, stderrWrite)) {
        closeHandle(stdoutWrite); closeHandle(state_->stdoutRead); return false;
    }

    std::vector<std::wstring> argv{spec.executable};
    argv.insert(argv.end(), spec.arguments.begin(), spec.arguments.end());
    std::wstring commandLine = buildWindowsCommandLine(argv);

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = stdoutWrite;
    startup.hStdError = stderrWrite;
    PROCESS_INFORMATION pi{};
    DWORD flags = CREATE_UNICODE_ENVIRONMENT | (spec.hideWindow ? CREATE_NO_WINDOW : 0);
    const wchar_t* cwd = spec.workingDirectory.empty() ? nullptr : spec.workingDirectory.c_str();

    BOOL created = CreateProcessW(spec.executable.c_str(), commandLine.data(), nullptr, nullptr, TRUE,
                                  flags, nullptr, cwd, &startup, &pi);
    closeHandle(stdoutWrite); closeHandle(stderrWrite);
    if (!created) { closeHandle(state_->stdoutRead); closeHandle(state_->stderrRead); return false; }

    state_->process = pi.hProcess;
    state_->thread = pi.hThread;
    state_->pid = pi.dwProcessId;
    state_->exitCode = STILL_ACTIVE;
    state_->running.store(true);

    auto readPipe = [this](HANDLE pipe, bool isStdErr) {
        char buffer[8192]; DWORD bytes = 0;
        while (ReadFile(pipe, buffer, sizeof(buffer), &bytes, nullptr) && bytes) {
            auto text = decodeChunk(buffer, bytes);
            if (state_->callback && !text.empty()) state_->callback(isStdErr, text);
        }
    };
    state_->stdoutReader = std::thread(readPipe, state_->stdoutRead, false);
    state_->stderrReader = std::thread(readPipe, state_->stderrRead, true);

    // The readers drain the pipes while this thread waits for process completion.
    // No detached thread is used: ProcessController owns all lifetime-sensitive state.
    std::thread([this] {
        WaitForSingleObject(state_->process, INFINITE);
        DWORD code = STILL_ACTIVE;
        GetExitCodeProcess(state_->process, &code);
        std::lock_guard lock(state_->mutex);
        state_->exitCode = code;
        state_->running.store(false);
    }).detach();
    return true;
}

bool ProcessController::terminate(UINT exitCode) {
    if (!state_ || !state_->process || !state_->running.load()) return true;
    return TerminateProcess(state_->process, exitCode) != FALSE;
}

bool ProcessController::wait(DWORD timeoutMs) {
    if (!state_ || !state_->process) return true;
    return WaitForSingleObject(state_->process, timeoutMs) == WAIT_OBJECT_0;
}

bool ProcessController::running() const noexcept { return state_ && state_->running.load(); }
DWORD ProcessController::processId() const noexcept { return state_ ? state_->pid : 0; }
DWORD ProcessController::exitCode() const noexcept { return state_ ? state_->exitCode : STILL_ACTIVE; }

} // namespace ytdlnis::process
