#pragma once

#include <Arduino.h>
#include "HSDConfig.h"
#include <ESP8266WebServer.h>

class HSDHtmlHelper
{

public:

  HSDHtmlHelper();

  String getHeader(const char* title, const char* host, const char* version) const;
  String getSaveButton() const;
  String getColorOptions(HSDConfig::Color selectedColor) const;
  String getBehaviorOptions(HSDConfig::Behavior selectedBehavior) const;
  String getTypeOptions(HSDConfig::deviceType selectedType) const;
  String getDeviceMappingEntry(int entryNum, const HSDConfig::deviceMapping* mapping) const;
  String getColorMappingEntry(int entryNum, const HSDConfig::colorMapping* mapping) const;
  String ip2String(IPAddress ip) const;
  String color2String(HSDConfig::Color color) const;
  String color2htmlColor(HSDConfig::Color color) const;
  String behavior2String(HSDConfig::Behavior behavior) const;
  String minutes2Uptime(unsigned long minutes) const;
};

