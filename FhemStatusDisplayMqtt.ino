#include "FhemStatusDisplayTypes.h"
#include "WifiConnector.h"
#include "MQTTHandler.h"
#include "LedSwitcher.h"
#include <Adafruit_NeoPixel.h>

// WLAN data
const char* WLAN_SSID =  "xxx";
const char* WLAN_PWD  =  "xxx";

// MQTT data
const char* MQTT_SERVER = "xxx";
const char* mqttInTopic = "xxx";

// LED data
const uint16_t NUMBER_OF_LEDS = 8;
const uint8_t LED_DATA_PIN = D2;

device devices[] =
{
  {TYPE_WINDOW, "bath_left",   7},
  {TYPE_WINDOW, "bath_right",  6},
  {TYPE_WINDOW, "child",       5},
  {TYPE_WINDOW, "sleep",       4},
  {TYPE_WINDOW, "work",        3},
  {TYPE_WINDOW, "kitchen",     2},
  {TYPE_DOOR,   "main",        1},
  {TYPE_DOOR,   "terrace",     0}
};

colorMapping colorMap[] = 
{
  {TYPE_WINDOW, "open",     {LedSwitcher::COLOR_BLUE,   LedSwitcher::MODE_ON}}, 
  {TYPE_WINDOW, "closed",   {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
  {TYPE_WINDOW, "tilted",   {LedSwitcher::COLOR_YELLOW, LedSwitcher::MODE_ON}}, 
  {TYPE_DOOR,   "open",     {LedSwitcher::COLOR_BLUE,   LedSwitcher::MODE_ON}}, 
  {TYPE_DOOR,   "closed",   {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
  {TYPE_DOOR,   "tilted",   {LedSwitcher::COLOR_YELLOW, LedSwitcher::MODE_ON}}, 
  {TYPE_LIGHT,  "on",       {LedSwitcher::COLOR_GREEN,  LedSwitcher::MODE_ON}}, 
  {TYPE_LIGHT,  "off",      {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
  {TYPE_ALARM,  "high",     {LedSwitcher::COLOR_RED,    LedSwitcher::MODE_FLASHING}}, 
  {TYPE_ALARM,  "low",      {LedSwitcher::COLOR_YELLOW, LedSwitcher::MODE_ON}}, 
  {TYPE_ALARM,  "off",      {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
};

// function declarations
void handleMqttMessage(String topic, String msg);

// Create class instances
WifiConnector wifiConnector(WLAN_SSID, WLAN_PWD);
MQTTHandler mqttHandler(MQTT_SERVER, 1883, mqttInTopic, handleMqttMessage);
Adafruit_NeoPixel leds(NUMBER_OF_LEDS, LED_DATA_PIN, NEO_GRB + NEO_KHZ800);
LedSwitcher ledSwitchers[NUMBER_OF_LEDS];

int getLedNumber(String deviceName, deviceType deviceType)
{
  int number = -1;

  for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(devices); i++)
  {
    if(deviceName.equals(devices[i].name) && (deviceType == devices[i].type))
    {
      number = devices[i].ledNumber;
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

void signalBootDone()
{
  for(uint32_t k = 0; k < 3; k++)
  {
    for(uint32_t i = 0; i < NUMBER_OF_LEDS; i++)
    {
      leds.setPixelColor(i, LedSwitcher::COLOR_GREEN);
      leds.show();
      delay(20);
      leds.setPixelColor(i, LedSwitcher::COLOR_OFF);
      leds.show();
      delay(20);
    }
    for(uint32_t i = NUMBER_OF_LEDS - 1; i >= 0; i--)
    {
      leds.setPixelColor(i, LedSwitcher::COLOR_GREEN);
      leds.show();
      delay(20);
      leds.setPixelColor(i, LedSwitcher::COLOR_OFF);
      leds.show();
      delay(20);
    }
  }
}

void setup() 
{ 
  Serial.begin(115200);
  wifiConnector.connect();
  leds.begin();
  
  for(uint32_t i=0; i<NUMBER_OF_LEDS; i++)
  {
    ledSwitchers[i].init(i, leds);
  }

  //signalBootDone();
}

void loop() 
{ 
  mqttHandler.handle();
  
  for(uint32_t i=0; i<NUMBER_OF_LEDS; i++)
  {
    ledSwitchers[i].update();
  }
}

