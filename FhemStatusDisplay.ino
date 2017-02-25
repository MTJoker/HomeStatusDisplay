#include "FhemStatusDisplayConfig.h"
#include "StatusDisplayLeds.h"
#include "WifiConnector.h"
#include "MQTTHandler.h"
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

// function declarations
void handleMqttMessage(String topic, String msg);

// Create class instances
FhemStatusDisplayConfig config;
WifiConnector wifiConnector(config);
MQTTHandler mqttHandler(config, handleMqttMessage);
StatusDisplayLeds leds(config);
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void handleMqttMessage(String topic, String msg)
{
  if(topic.equals(config.getMqttTestTopic()))
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
  int ledNumber = config.getLedNumber(device, type);
  int colorMapIndex = config.getColorMapIndex(type, msg);

  if( (ledNumber != -1) && (colorMapIndex != -1) )
  {
    Led::Behavior behavior = config.getLedBehavior(colorMapIndex);
    Led::Color color = config.getLedColor(colorMapIndex);

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
  Serial.begin(115200);
  config.begin();
  wifiConnector.connect();
  mqttHandler.begin();
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  leds.begin();
  
  leds.test(4);
}

void loop() 
{     
  wifiConnector.checkConnection();
  httpServer.handleClient(); 
  mqttHandler.handle();
  leds.update();
}

