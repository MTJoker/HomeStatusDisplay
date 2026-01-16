#include "HSDMqtt.hpp"

HSDMqtt::HSDMqtt(const HSDConfig& config, MQTT_CALLBACK_SIGNATURE)
    : m_pubSubClient(m_wifiClient)
    , m_config(config)
{
    m_pubSubClient.setCallback(callback);
}

void HSDMqtt::begin()
{
    IPAddress mqttIpAddr;
    const char* server = m_config.getMqttServer();

    if(mqttIpAddr.fromString(server))
    {
        m_pubSubClient.setServer(mqttIpAddr, MqttPort);
    }
    else
    {
        m_pubSubClient.setServer(server, MqttPort);
    }
}

void HSDMqtt::handle()
{
    if(isConnected())
    {
        m_pubSubClient.loop();
        return;
    }

    const unsigned long now = millis();
    if((now - m_millisLastConnectTry) < RetryDelayMs)
    {
        return;
    }

    m_millisLastConnectTry = now;

    if(reconnect())
    {
        Serial.println(F("MQTT reconnect successful"));
    }
    else
    {
        Serial.println(F("MQTT reconnect failed"));
    }
}

bool HSDMqtt::isConnected() const
{
    return m_pubSubClient.connected();
}

bool HSDMqtt::reconnect()
{
    if(!m_config.hasMqttConfig())
    {
        Serial.println(F("No MQTT config available."));
        return false;
    }

    String clientId = F("HomeStatusDisplay-");
    clientId += String(ESP.getChipId(), HEX);

    Serial.print(F("Connecting to MQTT broker "));
    Serial.print(m_config.getMqttServer());
    Serial.print(F(" with client id "));
    Serial.print(clientId);
    Serial.print(F("... "));

    const auto willTopic = m_config.getMqttWillTopic();
    const auto user = m_config.getMqttUser();
    const auto password = m_config.getMqttPassword();

    const auto useWill = isTopicValid(willTopic);
    const auto useAuth = isUserValid(user);

    bool connected = false;

    if(useAuth)
    {
        connected = useWill
                        ? m_pubSubClient.connect(clientId.c_str(), user, password, willTopic, 0, true, "offline")
                        : m_pubSubClient.connect(clientId.c_str(), user, password);
    }
    else
    {
        connected = useWill
                        ? m_pubSubClient.connect(clientId.c_str(), willTopic, 0, true, "offline")
                        : m_pubSubClient.connect(clientId.c_str());
    }

    if(!connected)
    {
        Serial.print(F("failed, rc="));
        Serial.println(m_pubSubClient.state());
        return false;
    }

    Serial.println(F("connected"));

    if(useWill)
    {
        publish(willTopic, "online");
    }

    subscribe(m_config.getMqttStatusTopic());
    subscribe(m_config.getMqttTestTopic());

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

void HSDMqtt::publish(const char* topic, const char* msg)
{
    if(m_pubSubClient.publish(topic, msg))
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

bool HSDMqtt::isTopicValid(const char* topic) const
{
    return topic && strlen(topic) > 0;
}

bool HSDMqtt::isUserValid(const char* user) const
{
    return user && strlen(user) > 0;
}
