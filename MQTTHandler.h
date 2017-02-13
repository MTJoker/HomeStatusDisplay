#ifndef _MQTT_HANDLER_
#define _MQTT_HANDLER_

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

typedef void (*mqttCallback)(String topic, String msg);

class MQTTHandler
{
public:

    MQTTHandler(IPAddress ip, uint16_t port, const char* inTopic, mqttCallback callback);
    MQTTHandler(const char * domain, uint16_t port, const char* inTopic, mqttCallback callback);
    MQTTHandler(const char * domain, uint16_t port);
    
    void handle();
    void publish(String topic, String msg);
    void addFwUpdateTopic(const char* topic);
    void addTestTopic(const char* topic);

private:

    void connectToMqttServer(); 
    void subscribe(const char* topic);
 
    WiFiClient m_wifiClient;
    PubSubClient m_pubSubClient;
    const char* m_domain;
    const char* m_inTopic;
    const char* m_fwUpdateTopic;
    const char* m_testTopic;
};

#endif
