#include "HSDDebug.hpp"
#include "HomeStatusDisplay.hpp"
#include <Arduino.h>

constexpr const char* Version = "1.0.0 beta";
constexpr const char* DefaultIdentifier = "HomeStatusDisplay";

std::unique_ptr<IHSDDebug> createDebug()
{
#if defined(HSD_ENABLE_DEBUG) && (HSD_ENABLE_DEBUG == 1)
    return std::make_unique<HSDDebug>();
#else
    return std::make_unique<HSDDebugNull>();
#endif
}

HomeStatusDisplay display(createDebug());

void setup()
{
    display.begin(Version, DefaultIdentifier);
}

void loop()
{
    display.loop();
}
