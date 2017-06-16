#pragma once

#include "HSDConfig.hpp"
#include "HSDWifi.hpp"
#include "HSDWebserver.hpp"
#include "HSDLeds.hpp"
#include "HSDMqtt.hpp"

class HomeStatusDisplay
{
public:

  HomeStatusDisplay();
  
  void begin(const char* version, const char* identifier);
  void work();
  
private:

  unsigned long calcUptime();

  static const int MQTT_MSG_MAX_LEN = 50;
  
  void mqttCallback(char* topic, byte* payload, unsigned int length);

  bool isStatusTopic(String& topic);
  HSDConfig::deviceType getDeviceType(String& statusTopic);
  String getDevice(String& statusTopic);

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
  unsigned long m_oneMinuteTimerLast;
  unsigned long m_uptime;
};

