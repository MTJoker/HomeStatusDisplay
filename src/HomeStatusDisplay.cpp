#include "HomeStatusDisplay.hpp"
#include "HSDEnumsToString.hpp"

const char* WINDOW_STRING = "/window/";
const char* DOOR_STRING = "/door/";
const char* LIGHT_STRING = "/light/";
const char* ALARM_STRING = "/alarm/";

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

        Serial.println("Uptime: " + String(m_uptime) + "min");
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

    if(std::strcmp(topic, m_config.getMqttTestTopic()) == 0)
    {
        handleTest(String(m_mqttMsgBuffer.data()));
        return;
    }

    if(isStatusTopic(topic))
    {
        const DeviceType type = getDeviceType(topic);
        const String device = getDevice(topic);

        handleStatus(device, type, String(m_mqttMsgBuffer.data()));
    }
}

bool HomeStatusDisplay::isStatusTopic(const String& topic) const
{
    const auto mqttStatusTopic = String(m_config.getMqttStatusTopic());
    const auto posOfLastSlashInStatusTopic = mqttStatusTopic.lastIndexOf("/");

    return topic.startsWith(mqttStatusTopic.substring(0, posOfLastSlashInStatusTopic));
}

DeviceType HomeStatusDisplay::getDeviceType(const String& statusTopic) const
{
    if(statusTopic.indexOf(LIGHT_STRING) != -1)
        return DeviceType::Light;
    if(statusTopic.indexOf(WINDOW_STRING) != -1)
        return DeviceType::Window;
    if(statusTopic.indexOf(DOOR_STRING) != -1)
        return DeviceType::Door;
    if(statusTopic.indexOf(ALARM_STRING) != -1)
        return DeviceType::Alarm;
    return DeviceType::Unknown;
}

String HomeStatusDisplay::getDevice(const String& statusTopic) const
{
    const auto posOfLastSlashInStatusTopic = statusTopic.lastIndexOf("/");
    return statusTopic.substring(posOfLastSlashInStatusTopic + 1);
}

void HomeStatusDisplay::handleTest(const String& msg)
{
    const auto type = msg.toInt();

    if(type > 0)
    {
        Serial.print(F("Showing testpattern "));
        Serial.println(type);
        m_leds.test(type);
    }
    else if(type == 0)
    {
        m_leds.clear();
        m_mqttHandler.reconnect(); // back to normal
    }
}

void HomeStatusDisplay::handleStatus(const String& device, DeviceType type, const String& msg)
{
    const int ledNumber = m_config.getLedNumber(device, type);

    if(ledNumber != -1)
    {
        const int colorMapIndex = m_config.getColorMapIndex(type, msg);

        if(colorMapIndex != -1)
        {
            const auto behavior = m_config.getLedBehavior(colorMapIndex);
            const auto color = m_config.getLedColor(colorMapIndex);

            Serial.println("Set led number " + String(ledNumber) + " to behavior " + toString(behavior) + " with color " + toString(color));
            m_leds.set(ledNumber, behavior, color);
        }
        else
        {
            Serial.println("Unknown message " + msg + " for led number " + String(ledNumber) + ", set to OFF");
            m_leds.set(ledNumber, Behavior::Off, Color::None);
        }
    }
    else
    {
        Serial.println("No LED defined for device " + device + " of type " + toString(type) + ", ignoring it");
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