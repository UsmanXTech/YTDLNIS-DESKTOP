#pragma once
#include "process/ProcessController.h"
#include "runtime/RuntimeManager.h"
#include "ytdl/CommandBuilder.h"
#include "ytdl/YtdlTypes.h"
#include <functional>
#include <memory>
#include <string>
namespace ytdlnis::ytdl {
struct ProgressEvent { double percent=-1.0; std::wstring downloaded; std::wstring total; std::wstring speed; std::wstring eta; std::wstring line; };
struct EngineResult { bool started=false; bool completed=false; unsigned long exitCode=0; std::wstring error; };
class YtdlEngine {
public:
 using ProgressCallback=std::function<void(const ProgressEvent&)>;
 using OutputCallback=std::function<void(bool,const std::wstring&)>;
 explicit YtdlEngine(runtime::RuntimeManager& runtime):runtime_(runtime){}
 EngineResult start(const YtdlRequest& request, ProgressCallback progress={}, OutputCallback output={});
 bool cancel();
private:
 runtime::RuntimeManager& runtime_;
 std::unique_ptr<process::ProcessController> process_;
};
}