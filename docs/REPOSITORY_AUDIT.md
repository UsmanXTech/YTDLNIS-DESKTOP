# Repository Audit Baseline

The reference repository is `deniscerri/ytdlnis`.

Key audited concepts:
- RuntimeManager: runtime discovery, process registry, executable setup, updates and environment construction.
- YTDLRequest/YTDLOptions: ordered option and argument construction.
- YTDLPUtil: metadata/format extraction, playlist handling, cookies, proxy/header settings, templates and yt-dlp request building.
- DownloadWorker: queue selection, concurrency, scheduling, delay, process execution, progress events, cache handling, output collection and history creation.
- DownloadRepository: active/paused/queued/cancelled/error/saved/scheduled state queries, concurrency and scheduling integration.
- DownloadItem: rich per-download state including format, preferences, subtitles, playlist info, templates, extra commands, destination, status and queue order.
- HistoryItem: URL, title, author, duration, thumbnail, type, timestamp, output paths, website, format, size, download ID and generated command.
- CommandTemplate: normal/audio/video/data-fetching extra-command modes, preferred templates and URL regex matching.
- Search history, result cache, logs, cookies and backup/restore are separate concerns.

## Native mapping
Android WorkManager -> native Scheduler/worker pool with optional Windows Task Scheduler integration.
Room -> SQLite.
ProcessBuilder -> CreateProcessW.
WorkerEventBus -> native application EventBus.
Android notifications -> Windows notification APIs.
Android storage/media scanning -> Windows Shell/filesystem APIs.
