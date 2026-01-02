#include "HSDMqtt.hpp"

HSDMqtt::HSDMqtt(const HSDConfig& config, MQTT_CALLBACK_SIGNATURE)
    : m_pubSubClient(m_wifiClient)
    , m_config(config)
{
    m_pubSubClient.setCallback(callback);
}

void HSDMqtt::begin()
{
    initTopics();

    addTopic(m_config.getMqttStatusTopic());
    addTopic(m_config.getMqttTestTopic());

    IPAddress mqttIpAddr;
    const char* server = m_config.getMqttServer();

    if(mqttIpAddr.fromString(server))
    {
        m_pubSubClient.setServer(mqttIpAddr, MQTT_PORT);
    }
    else
    {
        m_pubSubClient.setServer(server, MQTT_PORT);
    }
}

void HSDMqtt::initTopics()
{
    for(auto& topic : m_inTopics)
    {
        topic = nullptr;
    }

    m_numberOfInTopics = 0;
}

void HSDMqtt::handle()
{
    if(connected())
    {
        m_pubSubClient.loop();
        return;
    }

    if(m_connectionFailed)
    {
        return;
    }

    const unsigned long now = millis();
    if((now - m_millisLastConnectTry) < RETRY_DELAY_MS)
    {
        return;
    }

    m_millisLastConnectTry = now;

    if(m_retryCount < MAX_CONNECT_RETRIES)
    {
        if(reconnect())
        {
            Serial.println(F("MQTT reconnect successful"));
            m_retryCount = 0;
        }
        else
        {
            ++m_retryCount;
            Serial.print(F("MQTT reconnect failed, retry "));
            Serial.println(m_retryCount);
        }
    }
    else
    {
        Serial.println(F("Failed to connect MQTT."));
        m_connectionFailed = true;
    }
}

bool HSDMqtt::connected() const
{
    return m_pubSubClient.connected();
}

bool HSDMqtt::reconnect()
{
    String clientId = F("ESP8266Client-");
    clientId += String(random(0xffff), HEX);

    Serial.print(F("Connecting to MQTT broker "));
    Serial.print(m_config.getMqttServer());
    Serial.print(F(" with client id "));
    Serial.print(clientId);
    Serial.print(F("... "));

    const char* willTopic = m_config.getMqttWillTopic();
    const bool useWill = isTopicValid(willTopic);

    const bool connected = useWill
                               ? m_pubSubClient.connect(clientId.c_str(), willTopic, 0, true, "off")
                               : m_pubSubClient.connect(clientId.c_str());

    if(!connected)
    {
        Serial.print(F("failed, rc="));
        Serial.println(m_pubSubClient.state());
        return false;
    }

    Serial.println(F("connected"));

    if(useWill)
    {
        publish(willTopic, F("on"));
    }

    for(uint32_t i = 0; i < m_numberOfInTopics; ++i)
    {
        subscribe(m_inTopics[i]);
    }

    return true;
}

void HSDMqtt::subscribe(const char* topic)
{
    if(!isTopicValid(topic))
    {
        return;
    }

    Serial.print(F("Subscribing to topic "));
    Serial.println(topic);

    if(!m_pubSubClient.subscribe(topic))
    {
        Serial.print(F("Failed to subscribe to topic "));
        Serial.println(topic);
    }
}

void HSDMqtt::publish(const String& topic, const String& msg)
{
    if(m_pubSubClient.publish(topic.c_str(), msg.c_str()))
    {
        Serial.print(F("Published "));
        Serial.print(msg);
        Serial.print(F(" to "));
        Serial.println(topic);
    }
    else
    {
        Serial.print(F("Publish failed for "));
        Serial.println(topic);
    }
}

bool HSDMqtt::addTopic(const char* topic)
{
    if(!isTopicValid(topic))
    {
        return false;
    }

    if(m_numberOfInTopics >= MAX_IN_TOPICS)
    {
        return false;
    }

    m_inTopics[m_numberOfInTopics++] = topic;
    
    return true;
}

bool HSDMqtt::isTopicValid(const char* topic) const
{
    return topic && topic[0] != '\0';
}
