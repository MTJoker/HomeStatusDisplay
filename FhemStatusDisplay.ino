#include "FhemStatusDisplayConfig.h"
#include "StatusDisplayLeds.h"
#include "WifiConnector.h"
#include "MQTTHandler.h"
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

// function declarations
void handleMqttMessage(String topic, String msg);

// Create class instances
WifiConnector wifiConnector(WLAN_SSID, WLAN_PWD);
MQTTHandler mqttHandler(MQTT_SERVER, 1883, MQTT_IN_TOPIC_STATUS, handleMqttMessage);
StatusDisplayLeds leds(NUMBER_OF_LEDS, LED_DATA_PIN);
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

int getLedNumber(String deviceName, deviceType deviceType)
{
  int number = -1;

  for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(deviceMap); i++)
  {
    if(deviceName.equals(deviceMap[i].name) && (deviceType == deviceMap[i].type))
    {
      number = deviceMap[i].ledNumber;
      break;
    }
  }

  return number;
}

int getColorMapIndex(deviceType deviceType, String msg)
{
  int index = -1;

  for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(colorMap); i++)
  {
    if(msg.equals(colorMap[i].msg) && (deviceType == colorMap[i].type))
    {
      index = i;
      break;
    }
  }

  return index;
}

Led::Behavior getLedBehavior(int colorMapIndex)
{
  return colorMap[colorMapIndex].behavior;
}

Led::Color getLedColor(int colorMapIndex)
{
  return colorMap[colorMapIndex].color;
}

void handleMqttMessage(String topic, String msg)
{
  if(topic.equals(MQTT_IN_TOPIC_TEST))
  {
    uint32_t type = msg.toInt();
    if(type > 0)
    {
      Serial.println("Showing testpattern " + String(type));
      leds.test(type);
    }
    else if(type == 0)
    {
      leds.clear();
      mqttHandler.reconnect();  // back to normal
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

void handleStatus(String device, deviceType type, String msg)
{
  int ledNumber = getLedNumber(device, type);
  int colorMapIndex = getColorMapIndex(type, msg);

  if( (ledNumber != -1) && (colorMapIndex != -1) )
  {
    Led::Behavior behavior = getLedBehavior(colorMapIndex);
    Led::Color color = getLedColor(colorMapIndex);

    Serial.println("Set led number " + String(ledNumber) + " to behavior " + String(behavior) + " with color " + String(color, HEX));
    leds.set(ledNumber, behavior, color);
  }
  else
  {
    Serial.println("No LED defined for device " + device + " of type " + String(type) + ", ignoring it");
  }
}

void setup() 
{ 
  mqttHandler.addTopic(MQTT_IN_TOPIC_TEST);
  Serial.begin(115200);
  wifiConnector.connect();
  httpUpdater.setup(&httpServer);
  httpServer.begin();

  leds.test(4);
}

void loop() 
{     
  wifiConnector.checkConnection();
  httpServer.handleClient(); 
  mqttHandler.handle();
  leds.update();
}

