#pragma once

#include "FhemStatusDisplayConfig.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

typedef void (*mqttCallback)(String topic, String msg);

class MQTTHandler
{
public:

  MQTTHandler(const FhemStatusDisplayConfig& config, mqttCallback callback);

  static const uint32_t MAX_IN_TOPICS = 10;

  void begin();
  void handle();
  void publish(String topic, String msg);
  bool addTopic(const char* topic);
  void reconnect();

private:

  void initTopics();
  void connectToMqttServer(); 
  void subscribe(const char* topic);

  WiFiClient m_wifiClient;
  PubSubClient m_pubSubClient;

  const FhemStatusDisplayConfig& m_config;

  const char* m_inTopics[MAX_IN_TOPICS];
  uint32_t m_numberOfInTopics;
};

