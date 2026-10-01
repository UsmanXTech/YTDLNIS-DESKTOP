cmake_minimum_required(VERSION 3.28)
project(YTDLnisDesktop VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT WIN32)
 message(FATAL_ERROR "YTDLnis Desktop is a Windows-only application")
endif()
add_executable(YTDLnis WIN32
 src/app/main.cpp
 src/process/CommandLine.cpp
 src/process/ProcessController.cpp
 src/ytdl/CommandBuilder.cpp
 src/runtime/RuntimeManager.cpp
 src/runtime/RuntimeDiagnostics.cpp
)
target_compile_definitions(YTDLnis PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(YTDLnis PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(YTDLnis PRIVATE user32 gdi32 ole32 shell32 shlwapi d2d1 dwrite windowscodecs)
add_subdirectory(tests)
