#include "HomeStatusDisplay.hpp"

// function declarations
void handleMqttMessage(const String& topic, const String& msg);

const char* WINDOW_STRING = "/window/";
const char* DOOR_STRING = "/door/";
const char* LIGHT_STRING = "/light/";
const char* ALARM_STRING = "/alarm/";

constexpr unsigned long ONE_MINUTE_MILLIS = 60000;

int getFreeRamSize();

HomeStatusDisplay::HomeStatusDisplay()
    : m_wifi(m_config)
    , m_webServer(m_config, m_leds, m_mqttHandler)
    , m_mqttHandler(m_config, std::bind(&HomeStatusDisplay::mqttCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
    , m_leds(m_config)
    , m_lastWifiConnectionState(false)
    , m_lastMqttConnectionState(false)
    , m_oneMinuteTimerLast(0)
    , m_uptime(0)
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

void HomeStatusDisplay::work()
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

void HomeStatusDisplay::mqttCallback(char* topic, byte* payload, unsigned int length)
{
    const auto copyLength = min(length, static_cast<unsigned int>(MQTT_MSG_MAX_LEN));

    for(unsigned int i = 0; i < copyLength; ++i)
    {
        mqttMsgBuffer[i] = payload[i];
    }
    mqttMsgBuffer[copyLength] = '\0';

    const String mqttTopicString(topic);
    const String mqttMsgString(mqttMsgBuffer);

    Serial.print(F("Received an MQTT message for topic "));
    Serial.println(mqttTopicString + ": " + mqttMsgString);

    if(mqttTopicString.equals(m_config.getMqttTestTopic()))
    {
        handleTest(mqttMsgString);
    }
    else if(isStatusTopic(mqttTopicString))
    {
        const auto type = getDeviceType(mqttTopicString);
        const auto device = getDevice(mqttTopicString);

        handleStatus(device, type, mqttMsgString);
    }
}

bool HomeStatusDisplay::isStatusTopic(const String& topic) const
{
    const auto mqttStatusTopic = String(m_config.getMqttStatusTopic());
    const auto posOfLastSlashInStatusTopic = mqttStatusTopic.lastIndexOf("/");

    return topic.startsWith(mqttStatusTopic.substring(0, posOfLastSlashInStatusTopic));
}

HSDConfig::deviceType HomeStatusDisplay::getDeviceType(const String& statusTopic) const
{
    if(statusTopic.indexOf(LIGHT_STRING) != -1)
        return HSDConfig::TYPE_LIGHT;
    if(statusTopic.indexOf(WINDOW_STRING) != -1)
        return HSDConfig::TYPE_WINDOW;
    if(statusTopic.indexOf(DOOR_STRING) != -1)
        return HSDConfig::TYPE_DOOR;
    if(statusTopic.indexOf(ALARM_STRING) != -1)
        return HSDConfig::TYPE_ALARM;

    return HSDConfig::TYPE_UNKNOWN;
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

void HomeStatusDisplay::handleStatus(const String& device, HSDConfig::deviceType type, const String& msg)
{
    const int ledNumber = m_config.getLedNumber(device, type);

    if(ledNumber != -1)
    {
        const int colorMapIndex = m_config.getColorMapIndex(type, msg);

        if(colorMapIndex != -1)
        {
            const auto behavior = m_config.getLedBehavior(colorMapIndex);
            const auto color = m_config.getLedColor(colorMapIndex);

            Serial.println("Set led number " + String(ledNumber) + " to behavior " + String(behavior) + " with color " + String(color, HEX));
            m_leds.set(ledNumber, behavior, color);
        }
        else
        {
            Serial.println("Unknown message " + msg + " for led number " + String(ledNumber) + ", set to OFF");
            m_leds.set(ledNumber, HSDConfig::OFF, HSDConfig::NONE);
        }
    }
    else
    {
        Serial.println("No LED defined for device " + device + " of type " + String(type) + ", ignoring it");
    }
}

void HomeStatusDisplay::checkConnections()
{
    if(!m_lastMqttConnectionState && m_mqttHandler.connected())
    {
        m_leds.clear();
        m_lastMqttConnectionState = true;
    }
    else if(m_lastMqttConnectionState && !m_mqttHandler.connected())
    {
        m_leds.clear();
        m_lastMqttConnectionState = false;
    }

    if(!m_mqttHandler.connected() && m_wifi.connected())
    {
        m_leds.setAll(HSDConfig::ON, HSDConfig::YELLOW);
    }

    if(!m_lastWifiConnectionState && m_wifi.connected())
    {
        m_leds.clear();

        if(!m_mqttHandler.connected())
        {
            m_leds.setAll(HSDConfig::ON, HSDConfig::YELLOW);
        }

        m_lastWifiConnectionState = true;
    }
    else if(m_lastWifiConnectionState && !m_wifi.connected())
    {
        m_leds.clear();
        m_lastWifiConnectionState = false;
    }

    if(!m_wifi.connected())
    {
        m_leds.setAll(HSDConfig::ON, HSDConfig::RED);
    }
}