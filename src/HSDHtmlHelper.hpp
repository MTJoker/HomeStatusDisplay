#pragma once

#include "HSDConfig.hpp"
#include <IPAddress.h>

class HSDHtmlHelper
{

  public:
    String getHeader(const char* title, const char* host, uint32_t reloadSeconds) const;
    String getFooter() const;

    String getColorMappingTableHeader() const;
    String getColorMappingTableEntry(int entryNum, const HSDConfig::ColorMapping& mapping) const;
    String getColorMappingTableFooter() const;
    String getColorMappingTableAddEntryForm(int newEntryNum, bool isFull) const;

    String getDeviceMappingTableHeader() const;
    String getDeviceMappingTableEntry(int entryNum, const HSDConfig::DeviceMapping& mapping) const;
    String getDeviceMappingTableFooter() const;
    String getDeviceMappingTableAddEntryForm(int newEntryNum, bool isFull) const;

    String getDeleteForm() const;
    String getSaveForm() const;

    String minutes2Uptime(unsigned long minutes) const;
    String ip2String(IPAddress ip) const;
    String color2htmlColor(Color color) const;

  private:
    String getColorOptions(Color selectedColor) const;
    String getBehaviorOptions(Behavior selectedBehavior) const;
    String getTypeOptions(DeviceType selectedType) const;
};
