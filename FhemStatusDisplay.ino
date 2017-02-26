#include "FhemStatusDisplay.h"

static const char* CONFIG_FILE_NAME = "/config.json";
static const char* VERSION = "0.1_dev";
static const char* IDENTIFIER = "StatusDisplay-GroundFloor";

FhemStatusDisplay display;

void setup() 
{ 
  display.begin(CONFIG_FILE_NAME, VERSION, IDENTIFIER);
}

void loop() 
{     
  display.work();
}

