#pragma once

#include "HSDConfig.hpp"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class HSDMqtt
{
  public:
    HSDMqtt(const HSDConfig& config, MQTT_CALLBACK_SIGNATURE);

    void begin();
    void handle();
    void publish(const char* topic, const char* msg);
    bool reconnect();
    bool connected() const;

  private:
    void subscribe(const char* topic);
    bool isTopicValid(const char* topic) const;
    bool isUserValid(const char* user) const;

    static constexpr unsigned long RetryDelayMs = 5000;
    static constexpr uint16_t MqttPort = 1883;

    WiFiClient m_wifiClient;
    mutable PubSubClient m_pubSubClient;

    const HSDConfig& m_config;

    bool m_connectionFailed = false;
    int m_retryCount = 0;
    unsigned long m_millisLastConnectTry = 0;
};
