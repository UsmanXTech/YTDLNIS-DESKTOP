# YTDLnis Desktop — Native Windows Architecture

## Goal
Build a native Windows desktop downloader inspired by YTDLnis, using C++20, Win32, Direct2D, DirectWrite, Windows APIs, CMake and MSVC.

## Behavioral reference
The Android YTDLnis project is the behavioral reference. Android-specific components are translated into native Windows equivalents rather than mechanically ported.

## Core layers
- UI: Win32 + Direct2D + DirectWrite
- Application: DownloadManager, Queue, History, Settings, Templates, Cookies, Scheduler, Notifications
- YTDL: YtdlRequest, YtdlOptions, CommandBuilder, MetadataFetcher, FormatFetcher, PlaylistFetcher, ProgressParser
- Process: CreateProcessW, asynchronous stdout/stderr, ProcessRegistry, cancellation
- Runtime: yt-dlp, FFmpeg/FFprobe, optional aria2c, optional JS runtimes
- Persistence: SQLite + migrations + repositories

## Runtime policy
Required: yt-dlp and FFmpeg/FFprobe.
Optional: aria2c, Deno, Node.js, QuickJS as dictated by current yt-dlp requirements.
Python is not a required application runtime.

## Data/state model
Download states: Active, Paused, Queued, Error, Cancelled, Saved, Processing, Scheduled, Duplicate.

## Process security
Use CreateProcessW with explicit argv handling. Do not use shell command strings, cmd.exe, PowerShell, system(), or popen() for normal downloads.

## Repository implementation phases
1. Complete source-level behavior/option audit
2. Native CMake skeleton
3. Win32/D2D/DWrite rendering foundation
4. Runtime manager
5. yt-dlp process engine
6. Persistence and recovery
7. Queue/download engine
8. Complete UI
9. Windows installer/integration
10. CI and verification
