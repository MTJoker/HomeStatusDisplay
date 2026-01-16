#include "HSDDebug.hpp"
#include <Arduino.h>

// survives reboot, but not power cycle
static HSDDebug::DebugData debugData __attribute__((section(".noinit")));

void HSDDebug::begin()
{
    if(debugData.magic != IHSDDebug::DebugData::MAGIC)
    {
        debugData = IHSDDebug::DebugData{};
        debugData.magic = IHSDDebug::DebugData::MAGIC;
    }
    debugData.bootCount++;
}

HSDDebug::DebugData HSDDebug::snapShot()
{
    return debugData;
}

void HSDDebug::setStep(LastStep step)
{
    debugData.lastStep = step;
    debugData.lastHeap = ESP.getFreeHeap();
    debugData.lastHeapFrag = ESP.getHeapFragmentation();
}
