#pragma once

#include "HSDConfig.hpp"
#include <Arduino.h>
#include <ESP8266WebServer.h>

class HSDHtmlHelper
{

  public:
    HSDHtmlHelper();

    String getHeader(const char* title, const char* host, const char* version, uint32_t reloadSeconds) const;
    String getFooter() const;

    String getColorMappingTableHeader() const;
    String getColorMappingTableEntry(int entryNum, const HSDConfig::ColorMapping* mapping) const;
    String getColorMappingTableFooter() const;
    String getColorMappingTableAddEntryForm(int newEntryNum, bool isFull) const;

    String getDeviceMappingTableHeader() const;
    String getDeviceMappingTableEntry(int entryNum, const HSDConfig::DeviceMapping* mapping) const;
    String getDeviceMappingTableFooter() const;
    String getDeviceMappingTableAddEntryForm(int newEntryNum, bool isFull) const;

    String getDeleteForm() const;
    String getSaveForm() const;

    String minutes2Uptime(unsigned long minutes) const;
    String ip2String(IPAddress ip) const;
    String color2htmlColor(HSDConfig::Color color) const;
    String behavior2String(HSDConfig::Behavior behavior) const;
    String color2String(HSDConfig::Color color) const;
    String type2String(HSDConfig::deviceType type) const;

  private:
    String getColorOptions(HSDConfig::Color selectedColor) const;
    String getBehaviorOptions(HSDConfig::Behavior selectedBehavior) const;
    String getTypeOptions(HSDConfig::deviceType selectedType) const;
};
