#include "HomeStatusDisplay.h"

// function declarations
void handleMqttMessage(String topic, String msg);

#define WINDOW_STRING (F("/window/"))
#define DOOR_STRING (F("/door/"))
#define LIGHT_STRING (F("/light/"))
#define ALARM_STRING (F("/alarm/"))

int getFreeRamSize();

HomeStatusDisplay::HomeStatusDisplay()
:
m_webServer(m_config),
m_wifi(m_config),
m_mqttHandler(m_config, std::bind(&HomeStatusDisplay::mqttCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
m_leds(m_config),
m_lastWifiConnectionState(false),
m_lastMqttConnectionState(false)
{
}

void HomeStatusDisplay::begin(const char* version, const char* identifier)
{
  // initialize serial
  Serial.begin(115200);
  Serial.println(F(""));

  m_config.begin(version, identifier);
  m_webServer.begin();
  m_leds.begin();
  m_wifi.begin();
  m_mqttHandler.begin(); 

  Serial.print(F("Free RAM: ")); Serial.println(ESP.getFreeHeap());
}

void HomeStatusDisplay::work()
{
  checkConnections();

  m_wifi.handleConnection();
  m_webServer.handleClient();

  if(m_wifi.connected())
  {
    m_mqttHandler.handle();
  }
  
  m_leds.update();

  delay(50);
}

void HomeStatusDisplay::mqttCallback(char* topic, byte* payload, unsigned int length)
{
  int i = 0;

  for(i = 0; (i < length) && (i < MQTT_MSG_MAX_LEN); i++) 
  {
    mqttMsgBuffer[i] = payload[i];
  }
  mqttMsgBuffer[i] = '\0';

  String mqttTopicString(topic);
  String mqttMsgString = String(mqttMsgBuffer);
  
  Serial.print(F("Received an MQTT message for topic ")); Serial.println(mqttTopicString + ": " + mqttMsgString);

  if(mqttTopicString.equals(m_config.getMqttTestTopic()))
  {
    handleTest(mqttMsgString);
  }
  else if(isStatusTopic(mqttTopicString))
  {
    HSDConfig::deviceType type = getDeviceType(mqttTopicString);
    String device = getDevice(mqttTopicString);

    handleStatus(device, type, mqttMsgString);
  }
}

bool HomeStatusDisplay::isStatusTopic(String& topic)
{
  String mqttStatusTopic = String(m_config.getMqttStatusTopic());
  int posOfLastSlashInStatusTopic = mqttStatusTopic.lastIndexOf("/");

  return topic.startsWith(mqttStatusTopic.substring(0, posOfLastSlashInStatusTopic)) ? true : false;
}

HSDConfig::deviceType HomeStatusDisplay::getDeviceType(String& statusTopic)
{
  HSDConfig::deviceType type = HSDConfig::TYPE_UNKNOWN;

  if(statusTopic.indexOf(LIGHT_STRING) != -1)
  {
    type = HSDConfig::TYPE_LIGHT;
  }
  else if(statusTopic.indexOf(WINDOW_STRING) != -1)
  {
    type = HSDConfig::TYPE_WINDOW;
  }
  else if(statusTopic.indexOf(DOOR_STRING) != -1)
  {
    type = HSDConfig::TYPE_DOOR;
  }
  else if(statusTopic.indexOf(ALARM_STRING) != -1)
  {
    type = HSDConfig::TYPE_ALARM;
  } 

  return type;
}

String HomeStatusDisplay::getDevice(String& statusTopic)
{
  int posOfLastSlashInStatusTopic = statusTopic.lastIndexOf("/");

  return statusTopic.substring(posOfLastSlashInStatusTopic + 1);
}

void HomeStatusDisplay::handleTest(String msg)
{
  int type = msg.toInt();
  if(type > 0)
  {
    Serial.print(F("Showing testpattern ")); Serial.println(type);
    m_leds.test(type);
  }
  else if(type == 0)
  {
    m_leds.clear();
    m_mqttHandler.reconnect();  // back to normal
  }
}

void HomeStatusDisplay::handleStatus(String device, HSDConfig::deviceType type, String msg)
{ 
  int ledNumber = m_config.getLedNumber(device, type);

  if(ledNumber != -1)
  {
    int colorMapIndex = m_config.getColorMapIndex(type, msg);
    
    if(colorMapIndex != -1)
    {
      HSDLed::Behavior behavior = m_config.getLedBehavior(colorMapIndex);
      HSDLed::Color color = m_config.getLedColor(colorMapIndex);

      Serial.println("Set led number " + String(ledNumber) + " to behavior " + String(behavior) + " with color " + String(color, HEX));
      m_leds.set(ledNumber, behavior, color);
    }
    else if(m_config.isSwitchLedOffIfUnknownMessage())
    {
      Serial.println("Unknown message " + msg + " for led number " + String(ledNumber) + ", set to OFF");
      m_leds.set(ledNumber, HSDLed::OFF, HSDLed::NONE);
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
    m_leds.setAll(HSDLed::ON, HSDLed::YELLOW);
  }
  
  if(!m_lastWifiConnectionState && m_wifi.connected())
  {
    m_leds.clear();

    if(!m_mqttHandler.connected())
    {
      m_leds.setAll(HSDLed::ON, HSDLed::YELLOW);
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
    m_leds.setAll(HSDLed::ON, HSDLed::RED);
  }
}


