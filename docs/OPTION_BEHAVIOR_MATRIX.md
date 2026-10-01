# YTDLnis Desktop — Option / Behavior Matrix

This document is the implementation contract for translating YTDLnis behavior to native Windows. It is a mapping specification, not a claim that these features are already implemented.

| Area | Android reference behavior | Native Windows design | Phase |
|---|---|---|---|
| URL/request | YTDL request object | `YtdlRequest` value model | 2 |
| Download type | video/audio/data/custom | `DownloadType` enum | 2 |
| Format | selected/combined format | typed format selector + raw yt-dlp format expression | 2 |
| Audio preference | codec/quality/container | `AudioPreferences` | 2 |
| Video preference | codec/quality/resolution/container | `VideoPreferences` | 2 |
| Output directory | configured destination | validated `std::filesystem::path` | 2 |
| Filename template | yt-dlp output template | `TemplateEngine` + validated template text | 2 |
| Extra arguments | command/template additions | structured argv tokens, never shell concatenation | 2 |
| Metadata | embed metadata | yt-dlp/FFmpeg option generation | 2 |
| Thumbnail | embed/save thumbnail | yt-dlp/FFmpeg option generation | 2 |
| Subtitles | available/selected subtitles | typed subtitle selection | 2 |
| Playlist | playlist URL/index/title | playlist policy + item model | 2 |
| Cookies | browser/file cookies | `CookieManager` with protected metadata | 2 |
| Proxy | proxy configuration | explicit proxy option model | 2 |
| Headers | custom headers | argv list / config structure | 2 |
| SponsorBlock | chapter/remove behavior | dedicated option model | 2 |
| Chapters | split/merge/output behavior | chapter policy | 2 |
| Rate limit | yt-dlp rate options | validated rate-limit model | 2 |
| Download sections | section selection | structured section expressions | 2 |
| Command templates | URL matching + preferred/extra modes | `TemplateManager` + regex matching | 2 |
| Data-fetch templates | separate data-fetch command mode | pre-download extraction pipeline | 2 |
| Delay | randomized download delay | scheduler delay policy | 2 |
| Concurrency | configurable active downloads | semaphore-backed scheduler | 2 |
| Scheduling | scheduled start time | scheduler + optional Task Scheduler wake-up | 2 |
| Metered network | optional network constraint | Windows network awareness | 2 |
| Progress | yt-dlp progress events | line/event parser + state reducer | 2 |
| ETA | parsed from process output | typed duration parser | 2 |
| Cancellation | worker/process cancellation | process tree/job-object cancellation | 2 |
| Pause | queue/process state | pause where supported; otherwise safe stop/resume policy | 2 |
| History | rich completed-download record | normalized SQLite history tables | 2 |
| Result cache | extraction results separate from history | SQLite result cache with expiry | 2 |
| Recovery | persisted download state | startup recovery manager | 2 |
| Runtime | managed yt-dlp/FFmpeg/etc. | per-user runtime directory + verification | 2 |
| JavaScript runtime | runtime may be needed by yt-dlp features | optional managed JS runtimes according to current yt-dlp requirements | 2 |
| Logs | worker/runtime logs | redacted structured application logs | 2 |

## Argument construction rule

The Windows implementation must produce an argument vector, not a shell command string:

```text
YtdlRequest
    -> OptionResolver
    -> CommandBuilder
    -> vector<wstring>
    -> CreateProcessW
```

The builder must keep executable path and arguments separate and must never depend on `cmd.exe`, PowerShell, `system()`, or `popen()` for normal downloads.

## Verification rule

Each row must eventually receive:

1. source-level evidence from the reference project;
2. a Windows implementation mapping;
3. unit tests where deterministic;
4. integration/manual tests where behavior depends on yt-dlp/network/media tools.

Until those exist, the feature is considered planned rather than implemented.
