#include "FhemStatusDisplay.h"

FhemStatusDisplay display;

void setup() 
{ 
  display.begin();
}

void loop() 
{     
  display.work();
}

