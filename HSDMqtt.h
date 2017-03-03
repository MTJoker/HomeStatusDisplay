#pragma once

#include "HSDConfig.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class HSDMqtt
{
public:

  HSDMqtt(const HSDConfig& config, MQTT_CALLBACK_SIGNATURE);

  static const uint32_t MAX_IN_TOPICS = 10;

  void begin();
  void handle();
  void publish(String topic, String msg);
  bool reconnect(); 
  bool addTopic(const char* topic);
  bool connected();

private:

  void initTopics();
  void subscribe(const char* topic);

  WiFiClient m_wifiClient;
  PubSubClient m_pubSubClient;

  const HSDConfig& m_config;

  const char* m_inTopics[MAX_IN_TOPICS];
  uint32_t m_numberOfInTopics;
  long m_lastReconnectAttempt;
};

