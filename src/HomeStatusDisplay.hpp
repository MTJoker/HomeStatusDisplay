#pragma once

#include "HSDConfig.hpp"
#include "HSDDebug.hpp"
#include "HSDEnums.hpp"
#include "HSDLeds.hpp"
#include "HSDMqtt.hpp"
#include "HSDWebserver.hpp"
#include "HSDWifi.hpp"

class HomeStatusDisplay
{
  public:
    HomeStatusDisplay(std::unique_ptr<IHSDDebug> debug);

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

    bool isStatusTopic(std::string_view topic) const;
    DeviceType getDeviceType(std::string_view statusTopic) const;
    std::string_view getDevice(std::string_view statusTopic) const;

    void handleStatus(std::string_view device, DeviceType type, std::string_view msg);
    void handleTest(std::string_view msg);

    void checkConnections();

    std::array<char, MqttMaxMsgLen + 1> m_mqttMsgBuffer{};

    HSDConfig m_config;
    HSDWifi m_wifi;
    HSDWebserver m_webServer;
    HSDMqtt m_mqttHandler;
    HSDLeds m_leds;

    std::unique_ptr<IHSDDebug> m_debug;

    ConnectionState m_lastWifiConnectionState = ConnectionState::Disconnected;
    ConnectionState m_lastMqttConnectionState = ConnectionState::Disconnected;
    unsigned long m_oneMinuteTimerLast = 0;
    unsigned long m_uptime = 0;
};