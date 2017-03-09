#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "HSDConfig.h"
#include "HSDLeds.h"

class HSDWebserver
{
  
public:

  HSDWebserver(HSDConfig& config, const HSDLeds& leds);

  void begin();
  void handleClient();

private:

  void deliverRootPage();
  void deliverStatusPage();
  void deliverColorMappingPage();
  void deliverDeviceMappingPage();
  void deliverNotFoundPage();

  void checkReboot();
  String htmlHeader(const char* title);
  String htmlSaveButton();
  String htmlColorOptions(HSDLed::Color selectedColor);
  String htmlTypeOptions(HSDConfig::deviceType selectedType);
  String htmlBehaviorOptions(HSDLed::Behavior selectedBehavior);
  String htmlColorMappingEntry(int entryNum, const HSDConfig::colorMapping* mapping);
  String htmlDeviceMappingEntry(int entryNum, const HSDConfig::deviceMapping* mapping);

  String ip2String(IPAddress ip);
  String color2String(HSDLed::Color color);
  String behavior2String(HSDLed::Behavior behavior);
  
  bool updateMainConfig();
  bool updateColorMappingConfig();
  bool updateDeviceMappingConfig();

  ESP8266WebServer m_server;
  ESP8266HTTPUpdateServer m_updateServer;
  HSDConfig& m_config;
  const HSDLeds& m_leds;
};
