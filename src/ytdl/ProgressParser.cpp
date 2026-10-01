#include "ytdl/ProgressParser.h"
#include <regex>
namespace ytdlnis::ytdl {
ProgressEvent ProgressParser::parse(const std::wstring& line) {
 ProgressEvent e; e.line=line;
 static const std::wregex percent(LR"((\d+(?:\.\d+)?)%)");
 static const std::wregex eta(LR"(ETA\s+([0-9:]+))",std::regex_constants::icase);
 static const std::wregex speed(LR"(at\s+([^\s]+/s))",std::regex_constants::icase);
 std::wsmatch m;
 if(std::regex_search(line,m,percent)) e.percent=std::stod(m[1].str());
 if(std::regex_search(line,m,eta)) e.eta=m[1].str();
 if(std::regex_search(line,m,speed)) e.speed=m[1].str();
 return e;
}
}