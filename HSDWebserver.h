#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "HSDConfig.h"
#include "HSDLeds.h"
#include "HSDMqtt.h"

class HSDWebserver
{
  
public:

  HSDWebserver(HSDConfig& config, const HSDLeds& leds, const HSDMqtt& mqtt);

  void begin();
  void handleClient(unsigned long deviceUptime);

private:

  void deliverRootPage();
  void deliverStatusPage();
  void deliverColorMappingPage();
  void deliverDeviceMappingPage();
  void deliverNotFoundPage();

  void checkReboot();
  String htmlHeader(const char* title);
  String htmlSaveButton();
  String htmlColorOptions(HSDConfig::Color selectedColor);
  String htmlTypeOptions(HSDConfig::deviceType selectedType);
  String htmlBehaviorOptions(HSDConfig::Behavior selectedBehavior);
  String htmlColorMappingEntry(int entryNum, const HSDConfig::colorMapping* mapping);
  String htmlDeviceMappingEntry(int entryNum, const HSDConfig::deviceMapping* mapping);

  String ip2String(IPAddress ip);
  String color2String(HSDConfig::Color color);
  String color2htmlColor(HSDConfig::Color color);
  String behavior2String(HSDConfig::Behavior behavior);
  String minutes2Uptime(unsigned long minutes);
  
  bool updateMainConfig();
  bool updateColorMappingConfig();
  bool updateDeviceMappingConfig();

  ESP8266WebServer m_server;
  ESP8266HTTPUpdateServer m_updateServer;
  HSDConfig& m_config;
  const HSDLeds& m_leds;
  const HSDMqtt& m_mqtt;
  unsigned long m_deviceUptimeMinutes;
};
