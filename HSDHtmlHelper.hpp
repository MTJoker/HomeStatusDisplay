#pragma once

#include <Arduino.h>
#include "HSDConfig.h"
#include <ESP8266WebServer.h>

class HSDHtmlHelper
{

public:

  HSDHtmlHelper();

  String getHeader(const char* title, const char* host, const char* version) const;
  String getFooter() const;

  String getColorMappingTableHeader() const;
  String getColorMappingTableEntry(int entryNum, const HSDConfig::colorMapping* mapping) const;
  String getColorMappingTableFooter() const;
  String getColorMappingTableAddEntryForm(int newEntryNum) const;

  String getDeviceMappingTableHeader() const;
  String getDeviceMappingTableEntry(int entryNum, const HSDConfig::deviceMapping* mapping) const;
  String getDeviceMappingTableFooter() const;
  String getDeviceMappingTableAddEntryForm(int entryNum) const;

  String getDeleteEntryForm() const;
  String getSaveForm() const;

  String minutes2Uptime(unsigned long minutes) const;
  String ip2String(IPAddress ip) const;
  String color2htmlColor(HSDConfig::Color color) const;
  String behavior2String(HSDConfig::Behavior behavior) const;
  String color2String(HSDConfig::Color color) const;
    
private:

  String getColorOptions(HSDConfig::Color selectedColor) const;
  String getBehaviorOptions(HSDConfig::Behavior selectedBehavior) const;
  String getTypeOptions(HSDConfig::deviceType selectedType) const;

  String type2String(HSDConfig::deviceType type) const;
};

