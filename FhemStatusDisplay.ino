#include "FhemStatusDisplay.h"

static const char* VERSION = "0.1_dev";
static const char* IDENTIFIER = "StatusDisplay";

FhemStatusDisplay display;

void setup() 
{ 
  display.begin(VERSION, IDENTIFIER);
}

void loop() 
{     
  display.work();
}

