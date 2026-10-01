#include "persistence/Database.h"
#include "persistence/MigrationManager.h"
#include <windows.h>
#include <sqlite3.h>
#include <filesystem>
#include <string>
namespace ytdlnis::persistence {
struct Database::Impl { explicit Impl(std::filesystem::path p):path(std::move(p)){} std::filesystem::path path; sqlite3* handle=nullptr; };
namespace { std::wstring wide(const char* s){if(!s)return{};int n=MultiByteToWideChar(CP_UTF8,0,s,-1,nullptr,0);if(n<=1)return{};std::wstring r(n-1,L'\0');MultiByteToWideChar(CP_UTF8,0,s,-1,r.data(),n);return r;} }
Database::Database(std::filesystem::path p):impl_(std::make_unique<Impl>(std::move(p))){ }
Database::~Database(){if(impl_->handle)sqlite3_close(impl_->handle);}
bool Database::open(std::wstring& error){std::error_code ec;std::filesystem::create_directories(impl_->path.parent_path(),ec);if(ec){error=L"Cannot create database directory";return false;}auto utf8=impl_->path.u8string();int rc=sqlite3_open_v2(reinterpret_cast<const char*>(utf8.c_str()),&impl_->handle,SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE,nullptr);if(rc!=SQLITE_OK){error=wide(sqlite3_errmsg(impl_->handle));if(impl_->handle)sqlite3_close(impl_->handle);impl_->handle=nullptr;return false;}return execute("PRAGMA foreign_keys=ON;",error)&&execute("PRAGMA journal_mode=WAL;",error);}
bool Database::migrate(std::wstring& error){if(!isOpen()){error=L"Database is not open";return false;}return MigrationManager::apply(*this,error);}
bool Database::execute(std::string_view sql,std::wstring& error){if(!impl_->handle){error=L"Database is not open";return false;}char* msg=nullptr;std::string statement(sql);int rc=sqlite3_exec(impl_->handle,statement.c_str(),nullptr,nullptr,&msg);if(rc!=SQLITE_OK){error=wide(msg?msg:sqlite3_errmsg(impl_->handle));if(msg)sqlite3_free(msg);return false;}return true;}
bool Database::isOpen()const noexcept{return impl_->handle!=nullptr;}
}