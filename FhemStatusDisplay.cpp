#include "FhemStatusDisplay.h"
#include <Arduino.h>

// function declarations
void handleMqttMessage(String topic, String msg);

FhemStatusDisplay::FhemStatusDisplay()
:
m_webServer(m_config),
m_wifi(m_config),
m_mqttHandler(m_config, std::bind(&FhemStatusDisplay::mqttCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
m_leds(m_config)
{
}

void FhemStatusDisplay::begin(const char* configFileName, const char* version, const char* identifier)
{
  // initialize serial
  Serial.begin(115200);
  Serial.println("");

  m_config.begin(configFileName, version, identifier);
  m_webServer.begin();
  m_leds.begin();
  m_wifi.begin();
  m_mqttHandler.begin(); 
  
  if(m_wifi.connected())
  {
    m_leds.test(4);
  }
}

void FhemStatusDisplay::work()
{
  if(!m_wifi.connected())
  {
    m_leds.set(32, Led::ON, Led::RED);
  }
  else
  {
    m_leds.set(32, Led::OFF, Led::NONE);
  }

  if(!m_mqttHandler.isConnected())
  {
    m_leds.set(31, Led::ON, Led::RED);
  }
  else
  {
    m_leds.set(31, Led::OFF, Led::NONE);
  }

  m_webServer.handleClient();
  m_mqttHandler.handle();
  m_leds.update();

  delay(50);
}

void FhemStatusDisplay::mqttCallback(char* topic, byte* payload, unsigned int length)
{
  int i = 0;

  for( i = 0; (i < length) && (i < MQTT_MSG_MAX_LEN); i++) 
  {
    mqttMsgBuffer[i] = payload[i];
  }
  mqttMsgBuffer[i] = '\0';

  String mqttTopicString(topic);
  String mqttMsgString = String(mqttMsgBuffer);
  
  Serial.println("Received an MQTT message for topic " + mqttTopicString + ": " + mqttMsgString);

  if(mqttTopicString.equals(m_config.getMqttTestTopic()))
  {
    handleTest(mqttMsgString);
  }
  else
  {
    if(mqttTopicString.substring(12, 17) == "light")
    {
      handleStatus(mqttTopicString.substring(18), TYPE_LIGHT, mqttMsgString);
    }
    else if(mqttTopicString.substring(12, 18) == "window")
    {
      handleStatus(mqttTopicString.substring(19), TYPE_WINDOW, mqttMsgString);
    }
    else if(mqttTopicString.substring(12, 16) == "door")
    {
      handleStatus(mqttTopicString.substring(17), TYPE_DOOR, mqttMsgString);
    }
    else if(mqttTopicString.substring(12, 17) == "alarm")
    {
      handleStatus(mqttTopicString.substring(18), TYPE_ALARM, mqttMsgString);
    }
    else
    {
      Serial.println("Unknown topic, ignoring");
    }
  }
}

void FhemStatusDisplay::handleTest(String msg)
{
  uint32_t type = msg.toInt();
  if(type > 0)
  {
    Serial.println("Showing testpattern " + String(type));
    m_leds.test(type);
  }
  else if(type == 0)
  {
    m_leds.clear();
    m_mqttHandler.reconnect();  // back to normal
  }
}

void FhemStatusDisplay::handleStatus(String device, deviceType type, String msg)
{
  int ledNumber = m_config.getLedNumber(device, type);
  int colorMapIndex = m_config.getColorMapIndex(type, msg);

  if( (ledNumber != -1) && (colorMapIndex != -1) )
  {
    Led::Behavior behavior = m_config.getLedBehavior(colorMapIndex);
    Led::Color color = m_config.getLedColor(colorMapIndex);

    Serial.println("Set led number " + String(ledNumber) + " to behavior " + String(behavior) + " with color " + String(color, HEX));
    m_leds.set(ledNumber, behavior, color);
  }
  else
  {
    Serial.println("No LED defined for device " + device + " of type " + String(type) + ", ignoring it");
  }
}
