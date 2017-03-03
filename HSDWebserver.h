#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "HSDConfig.h"

class HSDWebserver
{
  
public:

  HSDWebserver(HSDConfig& config);

  void begin();
  void handleClient();

private:

  void deliverRootPage();
  void deliverColorMappingPage();
  void deliverDeviceMappingPage();
  void deliverNotFoundPage();

  String getHeader(const char* title);
  String ip2String(IPAddress ip);
  bool updateMainConfig();
  bool updateColorMappingConfig();
  bool updateDeviceMappingConfig();

  ESP8266WebServer m_server;
  ESP8266HTTPUpdateServer m_updateServer;
  HSDConfig& m_config;
};
