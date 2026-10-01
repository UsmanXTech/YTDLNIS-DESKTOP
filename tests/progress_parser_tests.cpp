#include "ytdl/ProgressParser.h"
#include <cassert>
#include <cmath>

using ytdlnis::ytdl::ProgressParser;

int main() {
    {
        const auto event = ProgressParser::parse(L"[download]  42.5% of 10.00MiB at 2.50MiB/s ETA 00:03");
        assert(std::abs(event.percent - 42.5) < 0.001);
        assert(event.speed == L"2.50MiB/s");
        assert(event.eta == L"00:03");
    }
    {
        const auto event = ProgressParser::parse(L"[download] 100% of 10.00MiB in 00:04 at 2.50MiB/s");
        assert(std::abs(event.percent - 100.0) < 0.001);
    }
    {
        const auto event = ProgressParser::parse(L"[download] Destination: output.mp4");
        assert(event.percent < 0.0);
        assert(event.eta.empty());
    }
    return 0;
}
