#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "HSDConfig.hpp"
#include "HSDLeds.hpp"
#include "HSDMqtt.hpp"
#include "HSDHtmlHelper.hpp"

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

  bool updateMainConfig();

  bool needAdd();
  bool needDelete();
  bool needDeleteAll();
  bool needSave();
  bool needUndo();
    
  bool addColorMappingEntry();
  bool deleteColorMappingEntry();

  bool addDeviceMappingEntry();
  bool deleteDeviceMappingEntry();
  
  bool updateDeviceMappingConfig();

  ESP8266WebServer m_server;
  ESP8266HTTPUpdateServer m_updateServer;
  HSDConfig& m_config;
  const HSDLeds& m_leds;
  const HSDMqtt& m_mqtt;
  unsigned long m_deviceUptimeMinutes;
  const HSDHtmlHelper m_html;
};
