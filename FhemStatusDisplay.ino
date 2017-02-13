#include "FhemStatusDisplayConfig.h"
#include "WifiConnector.h"
#include "MQTTHandler.h"
#include "LedSwitcher.h"
#include "Heartbeat.h"
#include <Adafruit_NeoPixel.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

static bool firmwareUpdate = false;

// function declarations
void handleMqttMessage(String topic, String msg);

// Create class instances
WifiConnector wifiConnector(WLAN_SSID, WLAN_PWD);
MQTTHandler mqttHandler(MQTT_SERVER, 1883, MQTT_IN_TOPIC_STATUS, handleMqttMessage);
Adafruit_NeoPixel leds(NUMBER_OF_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
LedSwitcher ledSwitchers[NUMBER_OF_LEDS];
Heartbeat heartbeat(HEARTBEAT_LED_PIN, HEARTBEAT_ON_TIME_DEFAULT, HEARTBEAT_OFF_TIME_DEFAULT);
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

LedSwitcher::ledState getLedState(deviceType deviceType, String msg)
{
  LedSwitcher::ledState state = {LedSwitcher::COLOR_OFF, LedSwitcher::MODE_OFF};

  for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(colorMap); i++)
  {
    if(msg.equals(colorMap[i].msg) && (deviceType == colorMap[i].type))
    {
      state = colorMap[i].state;
      break;
    }
  }

  return state;
}

void handleMqttMessage(String topic, String msg)
{
  if(topic.equals(MQTT_IN_TOPIC_FW_UPDATE))
  {
    if(msg.equals("on"))
    {
      Serial.println("Switch to OTA firmware update mode");
      heartbeat.changeTiming(HEARTBEAT_ON_TIME_FWUPDATE, HEARTBEAT_OFF_TIME_FWUPDATE);
      firmwareUpdate = true;
    }
  }
  else if(topic.equals(MQTT_IN_TOPIC_TEST))
  {
    uint32_t numberOfTimes = msg.toInt();
    if(numberOfTimes > 0)
    {
      Serial.println("Showing testpattern " + String(numberOfTimes) + " times");
      doLedStripeTestPattern(numberOfTimes);
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
  uint32_t ledNumber = getLedNumber(device, type);

  if(ledNumber != -1)
  {
    LedSwitcher::ledState state = getLedState(type, msg);
    ledSwitchers[ledNumber].set(state);
  }
  else
  {
    Serial.println("No LED defined for device " + device + " of type " + String(type) + ", ignoring it");
  }
}

void doLedStripeTestPattern(uint32_t numberOfCycles)
{
  uint32_t colors[] = {LedSwitcher::COLOR_RED, LedSwitcher::COLOR_GREEN, LedSwitcher::COLOR_BLUE};

  for(uint32_t cycle = 0; cycle < numberOfCycles; cycle++)
  {
    for(uint32_t colorIndex = 0; colorIndex < NUMBER_OF_ELEMENTS(colors); colorIndex++)
    {
      for(uint32_t i = 0; i < NUMBER_OF_LEDS; i++)
      {
        leds.setPixelColor(i, colors[colorIndex]);
        leds.show();
        delay(50);
      }
    }
  
    for(uint32_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      leds.setPixelColor(i, LedSwitcher::COLOR_OFF);
      leds.show();
    }
  }
}

void setup() 
{ 
  pinMode(HEARTBEAT_LED_PIN, OUTPUT);
  mqttHandler.addFwUpdateTopic(MQTT_IN_TOPIC_FW_UPDATE);
  mqttHandler.addTestTopic(MQTT_IN_TOPIC_TEST);
  Serial.begin(115200);
  wifiConnector.connect();
  leds.begin();
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  
  for(uint32_t i=0; i<NUMBER_OF_LEDS; i++)
  {
    ledSwitchers[i].init(i, leds);
  }

  doLedStripeTestPattern(1);
}

void loop() 
{     
  wifiConnector.checkConnection();

  if(firmwareUpdate)
  {
    httpServer.handleClient();
  }
  else
  {   
    mqttHandler.handle();
    
    for(uint32_t i=0; i<NUMBER_OF_LEDS; i++)
    {
      ledSwitchers[i].update();
    }
  }

  heartbeat.update();
}

