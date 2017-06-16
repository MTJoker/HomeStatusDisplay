#include "HSDMqtt.hpp"

HSDMqtt::HSDMqtt(const HSDConfig& config, MQTT_CALLBACK_SIGNATURE)
:
m_config(config),
m_pubSubClient(m_wifiClient),
m_maxConnectRetries(3),
m_numConnectRetriesDone(0),
m_retryDelay(5000),
m_millisLastConnectTry(0),
m_numberOfInTopics(0)
{
  m_pubSubClient.setCallback(callback);
}

void HSDMqtt::begin()
{  
  initTopics();
  addTopic(m_config.getMqttStatusTopic());
  addTopic(m_config.getMqttTestTopic());

  IPAddress mqttIpAddr;

  if(mqttIpAddr.fromString(m_config.getMqttServer()))
  {
    // valid ip address entered 
    m_pubSubClient.setServer(mqttIpAddr, 1883); 
  }
  else
  {
    // invalid ip address, try as hostname
    m_pubSubClient.setServer(m_config.getMqttServer(), 1883);  
  }
}

void HSDMqtt::initTopics()
{
  for(uint32_t index = 0; index < MAX_IN_TOPICS; index++)
  {
    m_inTopics[index] = NULL;
  }

  m_numberOfInTopics = 0;
}

void HSDMqtt::handle()
{
  if(connected())
  {
    m_pubSubClient.loop();
  }
  else if(!m_connectFailure) 
  {
    unsigned long currentMillis = millis();

    if( (currentMillis - m_millisLastConnectTry) >= m_retryDelay)
    {
      m_millisLastConnectTry = currentMillis; 

      if(m_numConnectRetriesDone < m_maxConnectRetries)
      {
        if(reconnect())
        {
          Serial.println("DEBUG: Reconnect successful");
          m_numConnectRetriesDone = 0;
        }
        else
        {
          m_numConnectRetriesDone++;
          Serial.println("DEBUG: Reconnect unsuccessful, m_numConnectRetriesDone = " + String(m_numConnectRetriesDone));
        }
      }
      else
      {
        Serial.println(F("Failed to connect Mqtt."));      
        m_connectFailure = true;
      } 
    }
  }
}

bool HSDMqtt::connected() const
{
  return m_pubSubClient.connected();
}

bool HSDMqtt::reconnect()
{
  bool retval = false;
  
  // Create a random client ID
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);

  Serial.print(F("Connecting to MQTT broker "));
  Serial.print(String(m_config.getMqttServer()));
  Serial.print(" with client id " + clientId + "... ");

  const char* willTopic = m_config.getMqttWillTopic();
  bool connected = false;
  
  if(isTopicValid(willTopic))
  {
    connected = m_pubSubClient.connect(clientId.c_str(), willTopic, 0, true, "off");
  }
  else
  {
    connected = m_pubSubClient.connect(clientId.c_str());
  }
  
  if(connected) 
  {
    Serial.println(F("connected"));

    if(isTopicValid(willTopic))
    {
      publish(willTopic, "on");
    }

    for(uint32_t index = 0; index < m_numberOfInTopics; index++)
    {
      subscribe(m_inTopics[index]);
    }

    retval = true;
  } 
  else 
  {
    Serial.print(F("failed, rc = "));
    Serial.println(m_pubSubClient.state());
  }

  return retval;
}

void HSDMqtt::subscribe(const char* topic)
{
  if(isTopicValid(topic))
  {
    Serial.print(F("Subscribing to topic "));
    Serial.println(topic);
    
    if(!m_pubSubClient.subscribe(topic))
    {
      Serial.print("Failed to subscribe to topic ");
      Serial.println(topic);
    }
  }
}

void HSDMqtt::publish(String topic, String msg)
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

bool HSDMqtt::addTopic(const char* topic)
{
  bool success = false;

  if(isTopicValid(topic))
  {  
    if(m_numberOfInTopics < (MAX_IN_TOPICS - 1))
    {
      m_inTopics[m_numberOfInTopics] = topic;
      m_numberOfInTopics++;
      success = true;
    }
  }

  return success;
}

bool HSDMqtt::isTopicValid(const char* topic)
{
  return (strlen(topic) > 0);
}

