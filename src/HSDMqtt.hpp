#pragma once

#include "HSDConfig.hpp"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class HSDMqtt
{
  public:
    static constexpr uint32_t MAX_IN_TOPICS = 10;

    HSDMqtt(const HSDConfig& config, MQTT_CALLBACK_SIGNATURE);

    void begin();
    void handle();
    void publish(const char* topic, const char* msg);
    bool reconnect();
    bool addTopic(const char* topic);
    bool connected() const;

  private:
    void initTopics();
    void subscribe(const char* topic);
    bool isTopicValid(const char* topic) const;
    bool isUserValid(const char* user) const;

    static constexpr unsigned long RETRY_DELAY_MS = 5000;
    static constexpr uint16_t MQTT_PORT = 1883;

    WiFiClient m_wifiClient;
    mutable PubSubClient m_pubSubClient;

    const HSDConfig& m_config;

    const char* m_inTopics[MAX_IN_TOPICS]{};
    uint32_t m_numberOfInTopics = 0;

    bool m_connectionFailed = false;
    int m_retryCount = 0;
    unsigned long m_millisLastConnectTry = 0;
};
