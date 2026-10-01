#include "persistence/Database.h"

#include <windows.h>
#include <sqlite3.h>
#include <filesystem>

namespace ytdlnis::persistence {

struct Database::Impl {
    explicit Impl(std::filesystem::path p) : path(std::move(p)) {}
    std::filesystem::path path;
    sqlite3* handle = nullptr;
};

namespace {
std::wstring utf8ToWide(const char* text) {
    if (!text) return L"";
    const int size = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
    if (size <= 0) return L"";
    std::wstring result(static_cast<std::size_t>(size - 1), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text, -1, result.data(), size);
    return result;
}
}

Database::Database(std::filesystem::path path) : impl_(std::make_unique<Impl>(std::move(path))) {}

Database::~Database() {
    if (impl_->handle) sqlite3_close(impl_->handle);
}

bool Database::open(std::wstring& error) {
    std::error_code ec;
    std::filesystem::create_directories(impl_->path.parent_path(), ec);
    if (ec) {
        error = L"Unable to create database directory: " + impl_->path.parent_path().wstring();
        return false;
    }

    const int rc = sqlite3_open_v2(
        impl_->path.u8string().c_str(), &impl_->handle,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

    if (rc != SQLITE_OK) {
        error = utf8ToWide(sqlite3_errmsg(impl_->handle));
        if (impl_->handle) sqlite3_close(impl_->handle);
        impl_->handle = nullptr;
        return false;
    }

    return execute("PRAGMA foreign_keys=ON;", error) &&
           execute("PRAGMA journal_mode=WAL;", error);
}

bool Database::migrate(std::wstring& error) {
    if (!isOpen()) {
        error = L"Database is not open";
        return false;
    }
    return MigrationManager::apply(*this, error);
}

bool Database::execute(std::string_view sql, std::wstring& error) {
    if (!impl_->handle) {
        error = L"Database is not open";
        return false;
    }

    char* message = nullptr;
    const int rc = sqlite3_exec(
        impl_->handle, std::string(sql).c_str(), nullptr, nullptr, &message);

    if (rc != SQLITE_OK) {
        error = utf8ToWide(message ? message : sqlite3_errmsg(impl_->handle));
        if (message) sqlite3_free(message);
        return false;
    }
    return true;
}

bool Database::isOpen() const noexcept { return impl_->handle != nullptr; }

} // namespace ytdlnis::persistence
