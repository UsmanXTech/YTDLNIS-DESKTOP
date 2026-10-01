#include "ytdl/YtdlEngine.h"
#include "ytdl/ProgressParser.h"
#include <mutex>
namespace ytdlnis::ytdl {
EngineResult YtdlEngine::start(const YtdlRequest& request, ProgressCallback progress, OutputCallback output) {
 EngineResult result;
 auto info=runtime_.discover(runtime::ToolKind::YtDlp);
 if(!info.available) { result.error=L"yt-dlp.exe was not found"; return result; }
 auto args=CommandBuilder::build(request,{info.path,runtime_.discover(runtime::ToolKind::Ffmpeg).path,runtime_.discover(runtime::ToolKind::Ffprobe).path});
 process_=std::make_unique<process::ProcessController>();
 process::ProcessSpec spec{info.path.wstring(),std::move(args),{},true};
 if(!process_->start(spec,[progress,output](bool err,const std::wstring& text){ if(output) output(err,text); if(!err && progress){ size_t start=0; while(start<text.size()){size_t end=text.find_first_of(L"\r\n",start); auto line=text.substr(start,end==std::wstring::npos?text.size()-start:end-start); if(!line.empty()) progress(ProgressParser::parse(line)); if(end==std::wstring::npos) break; start=end+1; } }})) { result.error=L"Failed to start yt-dlp"; process_.reset(); return result; }
 result.started=true;
 if(!process_->wait(INFINITE)) { result.error=L"Failed waiting for yt-dlp"; return result; }
 result.exitCode=process_->exitCode(); result.completed=result.exitCode==0;
 if(!result.completed) result.error=L"yt-dlp exited with a non-zero status";
 return result;
}
bool YtdlEngine::cancel(){return process_ ? process_->terminate(1) : true;}
}