#include "persistence/MigrationManager.h"

namespace ytdlnis::persistence {

bool MigrationManager::apply(Database& db, std::wstring& error) {
    if (!db.execute("CREATE TABLE IF NOT EXISTS schema_version (version INTEGER NOT NULL);", error)) return false;
    if (!db.execute("CREATE TABLE IF NOT EXISTS downloads (id INTEGER PRIMARY KEY, url TEXT NOT NULL, state INTEGER NOT NULL, percent REAL NOT NULL DEFAULT -1, speed TEXT, eta TEXT, error TEXT, created_at INTEGER NOT NULL, updated_at INTEGER NOT NULL);", error)) return false;
    if (!db.execute("CREATE TABLE IF NOT EXISTS queue_items (download_id INTEGER PRIMARY KEY REFERENCES downloads(id) ON DELETE CASCADE, position INTEGER NOT NULL, active INTEGER NOT NULL DEFAULT 1);", error)) return false;
    if (!db.execute("CREATE TABLE IF NOT EXISTS history (download_id INTEGER PRIMARY KEY REFERENCES downloads(id) ON DELETE CASCADE, completed_at INTEGER NOT NULL);", error)) return false;
    if (!db.execute("CREATE TABLE IF NOT EXISTS settings (key TEXT PRIMARY KEY, value TEXT NOT NULL);", error)) return false;
    if (!db.execute("CREATE TABLE IF NOT EXISTS templates (id INTEGER PRIMARY KEY, name TEXT NOT NULL UNIQUE, value TEXT NOT NULL, created_at INTEGER NOT NULL, updated_at INTEGER NOT NULL);", error)) return false;
    if (!db.execute("DELETE FROM schema_version;", error)) return false;
    return db.execute("INSERT INTO schema_version(version) VALUES(1);", error);
}

} // namespace ytdlnis::persistence
