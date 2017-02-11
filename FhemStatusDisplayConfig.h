#ifndef _FHEM_STATUS_DISPLAY_CONFIG_H_
#define _FHEM_STATUS_DISPLAY_CONFIG_H_

#include "FhemStatusDisplayTypes.h"

/*
 * WLAN config
 */
const char* WLAN_SSID =  "ADD_YOUR_WLAN_SSID_HERE";
const char* WLAN_PWD  =  "ADD_YOUR_WLAN_PASSWORD_HERE";

/*
 * MQTT config
 */
const char* MQTT_SERVER = "ADD_YOUR_MQTT_SERVER_NAME_HERE";
const char* MQTT_IN_TOPIC = "ADD_YOUR_SUBSCRIPTION_TOPIC_HERE";

/*
 * LED config
 */
const uint16_t NUMBER_OF_LEDS = 8;
const uint8_t LED_DATA_PIN = D2;

/*
 * List of known devices with mapping to led number
 */
deviceMapping deviceMap[] =
{
  {"bath_left",  TYPE_WINDOW, 7},
  {"bath_right", TYPE_WINDOW, 6},
  {"child",      TYPE_WINDOW, 5},
  {"sleep",      TYPE_WINDOW, 4},
  {"work",       TYPE_WINDOW, 3},
  {"kitchen",    TYPE_WINDOW, 2},
  {"main",       TYPE_DOOR,   1},
  {"terrace",    TYPE_DOOR,   0}
};

/*
 * List of messages for specific types with mapping to led state
 */
colorMapping colorMap[] = 
{
  {"open",   TYPE_WINDOW, {LedSwitcher::COLOR_BLUE,   LedSwitcher::MODE_ON}}, 
  {"closed", TYPE_WINDOW, {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
  {"tilted", TYPE_WINDOW, {LedSwitcher::COLOR_YELLOW, LedSwitcher::MODE_ON}}, 
  {"open",   TYPE_DOOR,   {LedSwitcher::COLOR_BLUE,   LedSwitcher::MODE_ON}}, 
  {"closed", TYPE_DOOR,   {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
  {"tilted", TYPE_DOOR,   {LedSwitcher::COLOR_YELLOW, LedSwitcher::MODE_ON}}, 
  {"on",     TYPE_LIGHT,  {LedSwitcher::COLOR_GREEN,  LedSwitcher::MODE_ON}}, 
  {"off",    TYPE_LIGHT,  {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
  {"high",   TYPE_ALARM,  {LedSwitcher::COLOR_RED,    LedSwitcher::MODE_BLINKING}}, 
  {"low",    TYPE_ALARM,  {LedSwitcher::COLOR_YELLOW, LedSwitcher::MODE_ON}}, 
  {"off",    TYPE_ALARM,  {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
};

#endif
