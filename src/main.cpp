#include "HSDDebug.hpp"
#include "HomeStatusDisplay.hpp"

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
    display.begin();
}

void loop()
{
    display.loop();
}
