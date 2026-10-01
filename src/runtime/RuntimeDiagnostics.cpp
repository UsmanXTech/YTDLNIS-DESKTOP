#include "runtime/RuntimeDiagnostics.h"
#include "process/ProcessController.h"
#include <filesystem>
namespace ytdlnis::runtime {
VersionResult detectVersion(const ToolInfo& tool) {
 if (!tool.available || tool.path.empty()) return {false,L"",L"Runtime executable is unavailable"};
 ytdlnis::process::ProcessController process;
 ytdlnis::process::ProcessSpec spec;
 spec.executable=tool.path.wstring(); spec.arguments={L"--version"}; spec.hideWindow=true;
 std::wstring output, error;
 if(!process.start(spec,[&](bool stderrStream,const std::wstring& text){(stderrStream?error:output)+=text;}))
   return {false,L"",L"CreateProcessW failed while starting runtime"};
 if(!process.wait(10000)) { process.terminate(); return {false,L"",L"Runtime version check timed out"}; }
 if(process.exitCode()!=0) return {false,L"",error.empty()?L"Runtime returned a non-zero exit code":error};
 while(!output.empty() && (output.back()==L'\r'||output.back()==L'\n'||output.back()==L' '||output.back()==L'\t')) output.pop_back();
 return {!output.empty(),output,output.empty()?L"Runtime returned no version text":L""};
}
}
