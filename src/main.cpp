#include "HomeStatusDisplay.hpp"
#include <Arduino.h>

constexpr const char* VERSION = "1.0.0 beta";
constexpr const char* IDENTIFIER = "HomeStatusDisplay";

HomeStatusDisplay display;

void setup()
{
    display.begin(VERSION, IDENTIFIER);
}

void loop()
{
    display.work();
}
