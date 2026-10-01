#pragma once
#include "persistence/Database.h"
#include <string>
namespace ytdlnis::persistence {
class MigrationManager {
public:
 static constexpr int CurrentVersion = 1;
 static bool apply(Database& database, std::wstring& error);
};
}