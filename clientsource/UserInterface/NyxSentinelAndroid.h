#pragma once
#include <string>

namespace NyxSentinelAndroid
{
    // Main-thread protocol bridge. The independent worker must complete real
    // executable-segment checks before its epoch can authorize a heartbeat.
    // This is not a port of the Windows-specific injection detectors.
    bool Pump(bool gameOnline, std::string& message);
}
