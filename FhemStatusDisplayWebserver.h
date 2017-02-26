#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "FhemStatusDisplayConfig.h"

class FhemStatusDisplayWebServer
{
  
public:

  FhemStatusDisplayWebServer(FhemStatusDisplayConfig& config);

  void begin();
  void handleClient();

private:

  void deliverRootPage();
  void deliverNotFoundPage();

  String ip2String(IPAddress ip);
  bool updateConfig();

  ESP8266WebServer m_server;
  ESP8266HTTPUpdateServer m_updateServer;
  FhemStatusDisplayConfig& m_config;
};