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
  {TYPE_WINDOW, "bath_left",   7},
  {TYPE_WINDOW, "bath_right",  6},
  {TYPE_WINDOW, "child",       5},
  {TYPE_WINDOW, "sleep",       4},
  {TYPE_WINDOW, "work",        3},
  {TYPE_WINDOW, "kitchen",     2},
  {TYPE_DOOR,   "main",        1},
  {TYPE_DOOR,   "terrace",     0}
};

/*
 * List of messages for specific types with mapping to led state
 */
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
  {TYPE_ALARM,  "high",     {LedSwitcher::COLOR_RED,    LedSwitcher::MODE_BLINKING}}, 
  {TYPE_ALARM,  "low",      {LedSwitcher::COLOR_YELLOW, LedSwitcher::MODE_ON}}, 
  {TYPE_ALARM,  "off",      {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
};

#endif
