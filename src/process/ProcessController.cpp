#include "process/ProcessController.h"
#include "ytdl/CommandLine.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <utility>

namespace ytdlnis::process {

namespace {

void closeHandle(HANDLE& handle) noexcept {
    if (handle != nullptr && handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
        handle = nullptr;
    }
}

bool createPipe(HANDLE& readHandle, HANDLE& writeHandle) {
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    if (!CreatePipe(&readHandle, &writeHandle, &sa, 0)) {
        return false;
    }

    // The parent must own the read end; only the child inherits the write end.
    if (!SetHandleInformation(readHandle, HANDLE_FLAG_INHERIT, 0)) {
        closeHandle(readHandle);
        closeHandle(writeHandle);
        return false;
    }
    return true;
}

} // namespace

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
    if (state_) {
        if (state_->stdoutReader.joinable()) state_->stdoutReader.join();
        if (state_->stderrReader.joinable()) state_->stderrReader.join();
        closeHandle(state_->thread);
        closeHandle(state_->process);
        closeHandle(state_->stdoutRead);
        closeHandle(state_->stderrRead);
        delete state_;
        state_ = nullptr;
    }
}

bool ProcessController::start(const ProcessSpec& spec, OutputCallback callback) {
    if (state_ && state_->running.load()) return false;

    if (!state_) state_ = new State();
    state_->callback = std::move(callback);

    HANDLE stdoutWrite = nullptr;
    HANDLE stderrWrite = nullptr;
    if (!createPipe(state_->stdoutRead, stdoutWrite)) return false;
    if (!createPipe(state_->stderrRead, stderrWrite)) {
        closeHandle(stdoutWrite);
        closeHandle(state_->stdoutRead);
        return false;
    }

    std::vector<std::wstring> argv;
    argv.reserve(spec.arguments.size() + 1);
    argv.push_back(spec.executable);
    argv.insert(argv.end(), spec.arguments.begin(), spec.arguments.end());
    std::wstring commandLine = buildWindowsCommandLine(argv);

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = stdoutWrite;
    startup.hStdError = stderrWrite;

    PROCESS_INFORMATION pi{};
    DWORD creationFlags = CREATE_UNICODE_ENVIRONMENT;
    if (spec.hideWindow) creationFlags |= CREATE_NO_WINDOW;

    std::wstring workingDirectory = spec.workingDirectory;
    const wchar_t* cwd = workingDirectory.empty() ? nullptr : workingDirectory.c_str();

    BOOL created = CreateProcessW(
        spec.executable.c_str(),
        commandLine.data(),
        nullptr,
        nullptr,
        TRUE,
        creationFlags,
        nullptr,
        cwd,
        &startup,
        &pi);

    closeHandle(stdoutWrite);
    closeHandle(stderrWrite);

    if (!created) {
        closeHandle(state_->stdoutRead);
        closeHandle(state_->stderrRead);
        return false;
    }

    state_->process = pi.hProcess;
    state_->thread = pi.hThread;
    state_->pid = pi.dwProcessId;
    state_->exitCode = STILL_ACTIVE;
    state_->running.store(true);

    auto readPipe = [this](HANDLE pipe, bool stderrStream) {
        char buffer[8192];
        DWORD bytesRead = 0;
        while (ReadFile(pipe, buffer, sizeof(buffer), &bytesRead, nullptr) && bytesRead != 0) {
            int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buffer,
                                            static_cast<int>(bytesRead), nullptr, 0);
            std::wstring text;
            if (count > 0) {
                text.resize(static_cast<size_t>(count));
                MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buffer,
                                    static_cast<int>(bytesRead), text.data(), count);
            } else {
                count = MultiByteToWideChar(CP_ACP, 0, buffer,
                                            static_cast<int>(bytesRead), nullptr, 0);
                text.resize(static_cast<size_t>(count));
                if (count > 0) {
                    MultiByteToWideChar(CP_ACP, 0, buffer,
                                        static_cast<int>(bytesRead), text.data(), count);
                }
            }
            if (state_->callback && !text.empty()) state_->callback(stderrStream, text);
        }
    };

    state_->stdoutReader = std::thread(readPipe, state_->stdoutRead, false);
    state_->stderrReader = std::thread(readPipe, state_->stderrRead, true);

    std::thread([this] {
        WaitForSingleObject(state_->process, INFINITE);
        DWORD code = STILL_ACTIVE;
        GetExitCodeProcess(state_->process, &code);
        {
            std::lock_guard lock(state_->mutex);
            state_->exitCode = code;
            state_->running.store(false);
        }
        closeHandle(state_->stdoutRead);
        closeHandle(state_->stderrRead);
    }).detach();

    return true;
}

bool ProcessController::terminate(UINT exitCode) {
    if (!state_ || !state_->process || !state_->running.load()) return true;
    return TerminateProcess(state_->process, exitCode) != FALSE;
}

bool ProcessController::wait(DWORD timeoutMs) {
    if (!state_ || !state_->process) return true;
    DWORD result = WaitForSingleObject(state_->process, timeoutMs);
    return result == WAIT_OBJECT_0;
}

bool ProcessController::running() const noexcept {
    return state_ && state_->running.load();
}

DWORD ProcessController::processId() const noexcept {
    return state_ ? state_->pid : 0;
}

DWORD ProcessController::exitCode() const noexcept {
    return state_ ? state_->exitCode : STILL_ACTIVE;
}

} // namespace ytdlnis::process
