#pragma once

#include "FhemStatusDisplayConfig.h"
#include "FhemStatusDisplayWifi.h"
#include "FhemStatusDisplayWebserver.h"
#include "StatusDisplayLeds.h"
#include "MQTTHandler.h"

class FhemStatusDisplay
{
public:

  FhemStatusDisplay();
  void begin(const char* version, const char* identifier);
  void work();
  
private:

  static const int MQTT_MSG_MAX_LEN = 50;
  
  void mqttCallback(char* topic, byte* payload, unsigned int length);

  void handleStatus(String device, deviceType type, String msg);
  void handleTest(String msg);

  void checkConnections();

  char mqttMsgBuffer[MQTT_MSG_MAX_LEN + 1];
  
  FhemStatusDisplayConfig m_config;
  FhemStatusDisplayWifi m_wifi;
  FhemStatusDisplayWebServer m_webServer;
  MQTTHandler m_mqttHandler;
  StatusDisplayLeds m_leds;

  bool m_lastWifiConnectionState;
  bool m_lastMqttConnectionState;
};

