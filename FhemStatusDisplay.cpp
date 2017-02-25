#include "FhemStatusDisplay.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>

const char* softApSsid = "FhemStatusDisplay";
const char* softApPw = "FhemStatusDisplay";

char mqttMsgBuffer[50];

// function declarations
void handleMqttMessage(String topic, String msg);

FhemStatusDisplay::FhemStatusDisplay()
:
m_webServer(m_config),
m_mqttHandler(m_config, std::bind(&FhemStatusDisplay::mqttCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
m_leds(m_config)
{
}

void FhemStatusDisplay::begin()
{
  // initialize serial
  Serial.begin(115200);
  Serial.println("");

  m_config.begin();
  m_webServer.begin();

  if(!startWifi())
  {
    startAccessPoint();
  }

  m_mqttHandler.begin();
  m_leds.begin();
  
  m_leds.test(4);
}

void FhemStatusDisplay::work()
{
  m_webServer.handleClient();
  m_mqttHandler.handle();
  m_leds.update();
}

void FhemStatusDisplay::startAccessPoint()
{
  Serial.println("");
  Serial.println("Starting access point.");
  
  if(WiFi.softAP(softApSsid, softApPw))
  {
    IPAddress ip = WiFi.softAPIP();
  
    Serial.printf("AccessPoint SSID is %s and IP is ", softApSsid);
    Serial.println(ip);
  }
  else
  {
    Serial.println("Error starting access point.");
  }
}

bool FhemStatusDisplay::startWifi()
{
  bool success = false;
  
  Serial.println("");
  Serial.print("Starting Wifi connection to ");
  Serial.print(m_config.getWifiSSID());

  WiFi.begin(m_config.getWifiSSID(), m_config.getWifiPSK());
  
  uint32_t timeout = 0;
  while( (WiFi.status() != WL_CONNECTED) && (timeout < 20) ) 
  {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  Serial.println("");
  
  if(WiFi.status() == WL_CONNECTED)
  {
    success = true; 
    Serial.print("WiFi connected with IP ");
    Serial.print(WiFi.localIP());
    Serial.println(".");
  }  
  else
  {
    Serial.println("Failed to connect WiFi.");
  }

  return success;
}

void FhemStatusDisplay::mqttCallback(char* topic, byte* payload, unsigned int length)
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

  handleMqttMessage(mqttTopicString, mqttMsgString);
}

void FhemStatusDisplay::handleMqttMessage(String topic, String msg)
{
  if(topic.equals(m_config.getMqttTestTopic()))
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
  else
  {
    if(topic.substring(12, 17) == "light")
    {
      handleStatus(topic.substring(18), TYPE_LIGHT, msg);
    }
    else if(topic.substring(12, 18) == "window")
    {
      handleStatus(topic.substring(19), TYPE_WINDOW, msg);
    }
    else if(topic.substring(12, 16) == "door")
    {
      handleStatus(topic.substring(17), TYPE_DOOR, msg);
    }
    else if(topic.substring(12, 17) == "alarm")
    {
      handleStatus(topic.substring(18), TYPE_ALARM, msg);
    }
    else
    {
      Serial.println("Unknown topic, ignoring");
    }
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
