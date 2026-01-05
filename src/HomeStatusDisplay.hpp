#pragma once

#include "HSDConfig.hpp"
#include "HSDEnums.hpp"
#include "HSDLeds.hpp"
#include "HSDMqtt.hpp"
#include "HSDWebserver.hpp"
#include "HSDWifi.hpp"

class HomeStatusDisplay
{
  public:
    HomeStatusDisplay();

    void begin(const char* version, const char* identifier);
    void work();

  private:
    unsigned long calcUptime();

    static constexpr int MQTT_MSG_MAX_LEN = 50;

    void mqttCallback(char* topic, byte* payload, unsigned int length);

    bool isStatusTopic(const String& topic) const;
    DeviceType getDeviceType(const String& statusTopic) const;
    String getDevice(const String& statusTopic) const;

    void handleStatus(const String& device, DeviceType type, const String& msg);
    void handleTest(const String& msg);

    void checkConnections();

    char mqttMsgBuffer[MQTT_MSG_MAX_LEN + 1]{};

    HSDConfig m_config;
    HSDWifi m_wifi;
    HSDWebserver m_webServer;
    HSDMqtt m_mqttHandler;
    HSDLeds m_leds;

    bool m_lastWifiConnectionState{false};
    bool m_lastMqttConnectionState{false};
    unsigned long m_oneMinuteTimerLast{0};
    unsigned long m_uptime{0};
};