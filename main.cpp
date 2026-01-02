#include "HomeStatusDisplay.hpp"
#include <Arduino.h>

constexpr const char* VERSION = "0.7_dev";
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
