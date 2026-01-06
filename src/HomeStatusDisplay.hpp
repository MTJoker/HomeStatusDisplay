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
    void loop();

  private:
    enum class ConnectionState : uint8_t
    {
        Disconnected,
        Connected
    };

    unsigned long calcUptime();

    static constexpr size_t MqttMaxMsgLen = 50;

    void mqttCallback(const char* topic, const uint8_t* payload, size_t length);

    bool isStatusTopic(const String& topic) const;
    DeviceType getDeviceType(const String& statusTopic) const;
    String getDevice(const String& statusTopic) const;

    void handleStatus(const String& device, DeviceType type, const String& msg);
    void handleTest(const String& msg);

    void checkConnections();

    std::array<char, MqttMaxMsgLen + 1> m_mqttMsgBuffer{};

    HSDConfig m_config;
    HSDWifi m_wifi;
    HSDWebserver m_webServer;
    HSDMqtt m_mqttHandler;
    HSDLeds m_leds;

    ConnectionState m_lastWifiConnectionState = ConnectionState::Disconnected;
    ConnectionState m_lastMqttConnectionState = ConnectionState::Disconnected;
    unsigned long m_oneMinuteTimerLast = 0;
    unsigned long m_uptime = 0;
};