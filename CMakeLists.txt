cmake_minimum_required(VERSION 3.28)
project(YTDLnisDesktop VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT WIN32)
 message(FATAL_ERROR "YTDLnis Desktop is a Windows-only application")
endif()

include(FetchContent)
FetchContent_Declare(
 sqlite3
 URL https://www.sqlite.org/2026/sqlite-amalgamation-3510000.zip
)
FetchContent_MakeAvailable(sqlite3)

add_library(sqlite3 STATIC
 ${sqlite3_SOURCE_DIR}/sqlite3.c
)
target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})
target_compile_definitions(sqlite3 PRIVATE SQLITE_THREADSAFE=1 SQLITE_DEFAULT_WAL_SYNCHRONOUS=1)

add_executable(YTDLnis WIN32
 src/app/main.cpp
 src/process/CommandLine.cpp
 src/process/ProcessController.cpp
 src/ytdl/CommandBuilder.cpp
 src/ytdl/YtdlEngine.cpp
 src/ytdl/ProgressParser.cpp
 src/runtime/RuntimeManager.cpp
 src/runtime/RuntimeDiagnostics.cpp
 src/download/DownloadJob.cpp
 src/download/DownloadQueue.cpp
 src/persistence/Database.cpp
 src/persistence/MigrationManager.cpp
)

target_compile_definitions(YTDLnis PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(YTDLnis PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(YTDLnis PRIVATE sqlite3 user32 gdi32 ole32 shell32 shlwapi d2d1 dwrite windowscodecs)

add_subdirectory(tests)
