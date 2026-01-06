#include "HomeStatusDisplay.hpp"
#include "HSDEnumsToString.hpp"
#include <charconv>

constexpr std::string_view WINDOW_STRING = "/window/";
constexpr std::string_view DOOR_STRING = "/door/";
constexpr std::string_view LIGHT_STRING = "/light/";
constexpr std::string_view ALARM_STRING = "/alarm/";

constexpr unsigned long ONE_MINUTE_MILLIS = 60000;

int getFreeRamSize();

HomeStatusDisplay::HomeStatusDisplay()
    : m_wifi(m_config)
    , m_webServer(m_config, m_leds, m_mqttHandler)
    , m_mqttHandler(m_config, [this](auto&&... args)
                    { mqttCallback(std::forward<decltype(args)>(args)...); })
    , m_leds(m_config)
{
}

void HomeStatusDisplay::begin(const char* version, const char* identifier)
{
    Serial.begin(115200);
    Serial.println();

    Serial.println(ESP.getResetReason());

    m_config.begin(version, identifier);
    m_webServer.begin();
    m_leds.begin();
    m_wifi.begin();
    m_mqttHandler.begin();

    Serial.print(F("Free RAM: "));
    Serial.println(ESP.getFreeHeap());
}

void HomeStatusDisplay::loop()
{
    const auto uptime = calcUptime();

    checkConnections();
    yield();

    m_wifi.handleConnection();
    yield();

    m_webServer.handleClient(uptime);
    yield();

    if(m_wifi.connected())
    {
        m_mqttHandler.handle();
        yield();
    }

    m_leds.update();
    delay(100);
}

unsigned long HomeStatusDisplay::calcUptime()
{
    const auto currentMillis = millis();

    if(currentMillis - m_oneMinuteTimerLast >= ONE_MINUTE_MILLIS)
    {
        m_uptime++;
        m_oneMinuteTimerLast = currentMillis;

        Serial.print("Uptime: ");
        Serial.print(m_uptime);
        Serial.println("min");
    }

    return m_uptime;
}

void HomeStatusDisplay::mqttCallback(const char* topic, const uint8_t* payload, size_t length)
{
    const size_t copyLength = std::min(length, m_mqttMsgBuffer.size() - 1);

    std::copy_n(reinterpret_cast<const char*>(payload), copyLength, m_mqttMsgBuffer.begin());
    m_mqttMsgBuffer[copyLength] = '\0';

    Serial.print(F("Received an MQTT message for topic "));
    Serial.print(topic);
    Serial.print(F(": "));
    Serial.println(m_mqttMsgBuffer.data());

    const std::string_view topicView{topic};

    if(topicView == m_config.getMqttTestTopic())
    {
        handleTest(m_mqttMsgBuffer.data());
    }
    else if(isStatusTopic(topicView))
    {
        const auto type = getDeviceType(topicView);
        const auto device = getDevice(topicView);

        handleStatus(device, type, m_mqttMsgBuffer.data());
    }
}

bool HomeStatusDisplay::isStatusTopic(std::string_view topic) const
{
    std::string_view statusTopic{m_config.getMqttStatusTopic()};

    const auto posOfLastSlash = statusTopic.rfind('/');
    std::string_view base = (posOfLastSlash == std::string_view::npos)
                                ? statusTopic
                                : statusTopic.substr(0, posOfLastSlash);

    if(topic.size() < base.size())
    {
        return false;
    }

    return std::memcmp(topic.data(), base.data(), base.size()) == 0;
}

DeviceType HomeStatusDisplay::getDeviceType(std::string_view statusTopic) const
{
    if(statusTopic.find(LIGHT_STRING) != std::string_view::npos)
        return DeviceType::Light;
    if(statusTopic.find(WINDOW_STRING) != std::string_view::npos)
        return DeviceType::Window;
    if(statusTopic.find(DOOR_STRING) != std::string_view::npos)
        return DeviceType::Door;
    if(statusTopic.find(ALARM_STRING) != std::string_view::npos)
        return DeviceType::Alarm;

    return DeviceType::Unknown;
}

std::string_view HomeStatusDisplay::getDevice(std::string_view statusTopic) const
{
    const auto posOfLastSlash = statusTopic.find_last_of('/');
    return (posOfLastSlash == std::string_view::npos)
               ? std::string_view{}
               : statusTopic.substr(posOfLastSlash + 1);
}

void HomeStatusDisplay::handleTest(std::string_view msg)
{
    int type = 0;
    std::from_chars(msg.data(), msg.data() + msg.size(), type);

    if(type > 0)
    {
        Serial.print(F("Showing testpattern "));
        Serial.println(type);
        m_leds.test(type);
    }
    else
    {
        m_leds.clear();
        m_mqttHandler.reconnect(); // back to normal
    }
}

void HomeStatusDisplay::handleStatus(std::string_view device, DeviceType type, std::string_view msg)
{
    const int ledNumber = m_config.getLedNumber(device, type);

    if(ledNumber != -1)
    {
        const int colorMapIndex = m_config.getColorMapIndex(type, msg);

        if(colorMapIndex != -1)
        {
            const auto behavior = m_config.getLedBehavior(colorMapIndex);
            const auto color = m_config.getLedColor(colorMapIndex);

            Serial.print(F("Set led number "));
            Serial.print(ledNumber);
            Serial.print(F(" to behavior "));
            Serial.print(toString(behavior));
            Serial.print(F(" with color "));
            Serial.println(toString(color));

            m_leds.set(ledNumber, behavior, color);
        }
        else
        {
            Serial.print(F("Unknown message "));
            Serial.print(msg.data());
            Serial.print(F(" for led number "));
            Serial.print(ledNumber);
            Serial.print(F(", setting it to "));
            Serial.println(toString(Behavior::Off));

            m_leds.set(ledNumber, Behavior::Off, Color::None);
        }
    }
    else
    {
        Serial.print(F("No LED defined for device "));
        Serial.print(device.data());
        Serial.print(F(" of type "));
        Serial.println(toString(type));
    }
}

void HomeStatusDisplay::checkConnections()
{
    const auto toState = [](bool connected)
    {
        return connected ? ConnectionState::Connected : ConnectionState::Disconnected;
    };

    const ConnectionState currentWifiState = toState(m_wifi.connected());
    const ConnectionState currentMqttState = toState(m_mqttHandler.connected());

    const bool wifiChanged = (currentWifiState != m_lastWifiConnectionState);
    const bool mqttChanged = (currentMqttState != m_lastMqttConnectionState);

    if(wifiChanged || mqttChanged)
    {
        m_leds.clear();
    }

    if(currentWifiState == ConnectionState::Disconnected)
    {
        m_leds.setAll(Behavior::On, Color::Red);
    }
    else if(currentMqttState == ConnectionState::Disconnected)
    {
        m_leds.setAll(Behavior::On, Color::Yellow);
    }

    m_lastWifiConnectionState = currentWifiState;
    m_lastMqttConnectionState = currentMqttState;
}