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

  static const uint32_t MQTT_MSG_MAX_LEN = 100;
  
  void mqttCallback(char* topic, byte* payload, unsigned int length);

  void handleStatus(String device, deviceType type, String msg);
  void handleTest(String msg);

  void startAccessPoint();
  bool startWifi();

  char mqttMsgBuffer[MQTT_MSG_MAX_LEN + 1];
  
  FhemStatusDisplayConfig m_config;
  FhemStatusDisplayWebServer m_webServer;
  MQTTHandler m_mqttHandler;
  StatusDisplayLeds m_leds;
};

