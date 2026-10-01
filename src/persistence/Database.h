#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
namespace ytdlnis::persistence {
class Database {
public:
 explicit Database(std::filesystem::path path);
 ~Database();
 Database(const Database&) = delete;
 Database& operator=(const Database&) = delete;
 bool open(std::wstring& error);
 bool migrate(std::wstring& error);
 bool execute(std::string_view sql, std::wstring& error);
 [[nodiscard]] bool isOpen() const noexcept;
private:
 struct Impl;
 std::unique_ptr<Impl> impl_;
};
}