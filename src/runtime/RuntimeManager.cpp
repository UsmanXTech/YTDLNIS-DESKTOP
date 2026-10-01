#include "runtime/RuntimeManager.h"
#include <windows.h>
#include <cstdlib>
namespace ytdlnis::runtime {
namespace {
std::filesystem::path localAppData() {
 wchar_t buffer[32768]; DWORD n=GetEnvironmentVariableW(L"LOCALAPPDATA",buffer,32768);
 return n ? std::filesystem::path(buffer) : std::filesystem::current_path();
}
std::filesystem::path pathFor(ToolKind k,const RuntimeLayout& l) {
 switch(k){case ToolKind::YtDlp:return l.ytDlp;case ToolKind::Ffmpeg:return l.ffmpeg;case ToolKind::Ffprobe:return l.ffmpeg.parent_path()/L"ffprobe.exe";case ToolKind::Aria2c:return l.aria2;case ToolKind::Deno:return l.deno;case ToolKind::Node:return l.node;case ToolKind::QuickJs:return l.quickJs;} return {};
}
std::wstring nameFor(ToolKind k){switch(k){case ToolKind::YtDlp:return L"yt-dlp.exe";case ToolKind::Ffmpeg:return L"ffmpeg.exe";case ToolKind::Ffprobe:return L"ffprobe.exe";case ToolKind::Aria2c:return L"aria2c.exe";case ToolKind::Deno:return L"deno.exe";case ToolKind::Node:return L"node.exe";case ToolKind::QuickJs:return L"qjs.exe";}return {};} 
}
RuntimeLayout RuntimeManager::defaultLayout(){return resolveLayout(std::nullopt);}
RuntimeLayout RuntimeManager::resolveLayout(const std::optional<std::filesystem::path>& root){
 RuntimeLayout l; l.root=root.value_or(localAppData()/L"YTDLnis"); l.runtime=l.root/L"runtime";
 l.ytDlp=l.runtime/L"yt-dlp"/L"yt-dlp.exe"; l.ffmpeg=l.runtime/L"ffmpeg"/L"ffmpeg.exe"; l.aria2=l.runtime/L"aria2"/L"aria2c.exe";
 l.node=l.runtime/L"node"/L"node.exe"; l.deno=l.runtime/L"deno"/L"deno.exe"; l.quickJs=l.runtime/L"quickjs"/L"qjs.exe";
 l.data=l.root/L"data"; l.cache=l.root/L"cache"; l.logs=l.root/L"logs"; l.cookies=l.root/L"cookies"; l.templates=l.root/L"templates"; return l;
}
RuntimeManager::RuntimeManager(RuntimeLayout layout):layout_(std::move(layout)){}
const RuntimeLayout& RuntimeManager::layout()const noexcept{return layout_;}
ToolInfo RuntimeManager::discover(ToolKind kind)const{
 ToolInfo info{kind}; info.path=pathFor(kind,layout_);
 if(std::filesystem::is_regular_file(info.path)){info.available=true;info.executable=true;return info;}
 wchar_t* path=nullptr; size_t len=0; if(_wdupenv_s(&path,&len,L"PATH")==0 && path){std::wstring p(path);free(path);size_t start=0;while(start<=p.size()){size_t end=p.find(L';',start);auto dir=p.substr(start,end==std::wstring::npos?p.size()-start:end-start);if(!dir.empty()){auto candidate=std::filesystem::path(dir)/nameFor(kind);if(std::filesystem::is_regular_file(candidate)){info.path=candidate;info.available=true;info.executable=true;break;}}if(end==std::wstring::npos)break;start=end+1;}}
 return info;
}
std::vector<ToolInfo> RuntimeManager::diagnostics()const{std::vector<ToolInfo> r;for(auto k:{ToolKind::YtDlp,ToolKind::Ffmpeg,ToolKind::Ffprobe,ToolKind::Aria2c,ToolKind::Deno,ToolKind::Node,ToolKind::QuickJs})r.push_back(discover(k));return r;}
} // namespace ytdlnis::runtime
