#pragma once

#include "HSDConfig.h"
#include "HSDWifi.h"
#include "HSDWebserver.h"
#include "HSDLeds.h"
#include "HSDMqtt.h"

class HomeStatusDisplay
{
public:

  HomeStatusDisplay();
  
  void begin(const char* version, const char* identifier);
  void work();
  
private:

  static const int MQTT_MSG_MAX_LEN = 50;
  
  void mqttCallback(char* topic, byte* payload, unsigned int length);

  void handleStatus(String device, HSDConfig::deviceType type, String msg);
  void handleTest(String msg);

  void checkConnections();

  char mqttMsgBuffer[MQTT_MSG_MAX_LEN + 1];
  
  HSDConfig m_config;
  HSDWifi m_wifi;
  HSDWebserver m_webServer;
  HSDMqtt m_mqttHandler;
  HSDLeds m_leds;

  bool m_lastWifiConnectionState;
  bool m_lastMqttConnectionState;
};

