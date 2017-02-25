#pragma once

#include "FhemStatusDisplayConfig.h"
#include "FhemStatusDisplayWebserver.h"
#include "StatusDisplayLeds.h"
#include "MQTTHandler.h"

class FhemStatusDisplay
{
public:

  FhemStatusDisplay();
  void begin();
  void work();
  
private:

  void mqttCallback(char* topic, byte* payload, unsigned int length);

  void handleMqttMessage(String topic, String msg);
  void handleStatus(String device, deviceType type, String msg);

  void startAccessPoint();
  bool startWifi();
  
  FhemStatusDisplayConfig m_config;
  FhemStatusDisplayWebServer m_webServer;
  MQTTHandler m_mqttHandler;
  StatusDisplayLeds m_leds;
};

