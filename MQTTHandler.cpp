#include "MQTTHandler.h"

static void callbackInternal(char* topic, byte* payload, unsigned int length);
char mqttMsgBuffer[20];
mqttCallback myCallback;

MQTTHandler::MQTTHandler(IPAddress ip, uint16_t port, const char* inTopic, mqttCallback callback)
:
m_pubSubClient(m_wifiClient),
m_domain(NULL),
m_inTopic(inTopic)
{
    myCallback = callback;
    m_pubSubClient.setServer(ip, port);
    m_pubSubClient.setCallback(callbackInternal);   
}

MQTTHandler::MQTTHandler(const char * domain, uint16_t port, const char* inTopic, mqttCallback callback)
:
m_pubSubClient(m_wifiClient),
m_domain(domain),
m_inTopic(inTopic)
{
    myCallback = callback;
    m_pubSubClient.setServer(domain, port);
    m_pubSubClient.setCallback(callbackInternal);
}

void MQTTHandler::handle()
{
  if (!m_pubSubClient.connected()) 
  {
    connectToMqttServer();
  }

  m_pubSubClient.loop();
}

void MQTTHandler::connectToMqttServer()
{
  // Loop until we're reconnected
  while (!m_pubSubClient.connected()) 
  { 
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    Serial.print("Connecting to MQTT broker ");
    if(m_domain) Serial.print(m_domain);
    Serial.print(" with client id " + clientId + "... ");
    
    // Attempt to connect
    if (m_pubSubClient.connect(clientId.c_str())) 
    {
      Serial.println("connected");

      if(m_inTopic != NULL)
      {
        Serial.print("Subscribing to topic ");
        Serial.println(m_inTopic);
        
        m_pubSubClient.subscribe(m_inTopic);
      }
      else
      {
        Serial.println("No topics to subscribe");
      }
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(m_pubSubClient.state());
      Serial.println(" try again in 5 seconds");
      
      delay(5000);  //retry after 5secs
    }
  }
}

void MQTTHandler::publish(String topic, String msg)
{
  if(m_pubSubClient.publish(topic.c_str(), msg.c_str()))
  {
    Serial.println("Published msg " + msg + " for topic " + topic);
  }
  else
  {
    Serial.println("Error publishing msg " + msg + " for topic " + topic);
  }
}

void callbackInternal(char* topic, byte* payload, unsigned int length) 
{ 
  int i = 0;

  for (i = 0; i < length; i++) 
  {
    mqttMsgBuffer[i] = payload[i];
  }
  mqttMsgBuffer[i] = '\0';

  String mqttTopicString(topic);
  String mqttMsgString = String(mqttMsgBuffer);
  
  Serial.println("Received an MQTT message for topic " + mqttTopicString + ": " + mqttMsgString);

  myCallback(mqttTopicString, mqttMsgString);
}

