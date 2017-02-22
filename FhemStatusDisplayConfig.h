#ifndef _FHEM_STATUS_DISPLAY_CONFIG_H_
#define _FHEM_STATUS_DISPLAY_CONFIG_H_

#include "FhemStatusDisplayTypes.h"

/*
 * WLAN config
 */
const char* WLAN_SSID =  "xxx";
const char* WLAN_PWD  =  "xxx";

/*
 * MQTT config
 */
const char* MQTT_SERVER = "xxx";
const char* MQTT_IN_TOPIC_STATUS    = "fhem/status/#";
const char* MQTT_IN_TOPIC_FW_UPDATE = "fhem/cmd/fwupdate/statusdisplay_01";
const char* MQTT_IN_TOPIC_TEST      = "fhem/cmd/test/statusdisplay_01";

/*
 * LED config
 */
const uint16_t NUMBER_OF_LEDS = 33;
const uint8_t LED_DATA_PIN = D2;
const uint8_t HEARTBEAT_LED_PIN = D1;

/*
 * Misc config
 */
const long HEARTBEAT_ON_TIME_DEFAULT = 300;
const long HEARTBEAT_OFF_TIME_DEFAULT = 3000;
const long HEARTBEAT_ON_TIME_FWUPDATE = 100;
const long HEARTBEAT_OFF_TIME_FWUPDATE = 100;

/*
 * List of known devices with mapping to led number
 */
deviceMapping deviceMap[] =
{
  // 1st row
  {"basement",   TYPE_DOOR,   10},
  {"main",       TYPE_DOOR,    9},
  {"kitchen",    TYPE_WINDOW,  8},
  {"eating",     TYPE_WINDOW,  7},
  {"terrace",    TYPE_DOOR,    6},
  {"bath_left",  TYPE_WINDOW,  5},
  {"bath_right", TYPE_WINDOW,  4},
  {"child",      TYPE_WINDOW,  3},
  {"sleep",      TYPE_WINDOW,  2},
  {"work",       TYPE_WINDOW,  1},
  {"garage",     TYPE_DOOR,    0},

  //2nd row
  {"kitchen_ceiling_right", TYPE_LIGHT, 21},
  {"kitchen_ceiling_left",  TYPE_LIGHT, 20},
  {"eating_ceiling",        TYPE_LIGHT, 19},
  {"living_ceiling",        TYPE_LIGHT, 18},
  {"living_stonewall",      TYPE_LIGHT, 17},
  {"living_lowboard",       TYPE_LIGHT, 16},
  {"living_shelf",          TYPE_LIGHT, 15},
  {"bath_ceiling",          TYPE_LIGHT, 14},
  {"child_ceiling",         TYPE_LIGHT, 13},
  {"sleep_ceiling",         TYPE_LIGHT, 12},
  {"work_ceiling",          TYPE_LIGHT, 11},

  //3rd row
  {"washing_machine",    TYPE_ALARM, 32},
  {"waterdetector_1",    TYPE_ALARM, 31},
  {"waterdetector_2",    TYPE_ALARM, 30},
  {"oven",               TYPE_ALARM, 29},
  {"waste_residual_bio", TYPE_ALARM, 28},
  {"waste_paper_yellow", TYPE_ALARM, 27},
  {"device_error",       TYPE_ALARM, 26},
  {"battery_error",      TYPE_ALARM, 25},
  {"unused_3",           TYPE_ALARM, 24},
  {"unused_2",           TYPE_ALARM, 23},
  {"unused_1",           TYPE_ALARM, 22},
};

/*
 * List of messages for specific types with mapping to led state
 */
colorMapping colorMap[] = 
{
  {"open",     TYPE_WINDOW, {LedSwitcher::COLOR_BLUE,   LedSwitcher::MODE_ON}}, 
  {"closed",   TYPE_WINDOW, {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
  {"tilted",   TYPE_WINDOW, {LedSwitcher::COLOR_YELLOW, LedSwitcher::MODE_ON}}, 
  {"open",     TYPE_DOOR,   {LedSwitcher::COLOR_BLUE,   LedSwitcher::MODE_ON}}, 
  {"closed",   TYPE_DOOR,   {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
  {"tilted",   TYPE_DOOR,   {LedSwitcher::COLOR_YELLOW, LedSwitcher::MODE_ON}}, 
  {"on",       TYPE_LIGHT,  {LedSwitcher::COLOR_GREEN,  LedSwitcher::MODE_ON}}, 
  {"off",      TYPE_LIGHT,  {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}, 
  {"true",     TYPE_ALARM,  {LedSwitcher::COLOR_YELLOW, LedSwitcher::MODE_BLINKING}}, // used by oven, defectDevices, battery
  {"false",    TYPE_ALARM,  {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}},      // used by oven, defectDevices, battery
  {"closed",   TYPE_ALARM,  {LedSwitcher::COLOR_RED,    LedSwitcher::MODE_BLINKING}}, // used by water detectors
  {"open",     TYPE_ALARM,  {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}},      // used by water detectors
  {"on",       TYPE_ALARM,  {LedSwitcher::COLOR_GREEN,  LedSwitcher::MODE_ON}},       // used by washing machine
  {"off",      TYPE_ALARM,  {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}},      // used by washing machine
  {"standby",  TYPE_ALARM,  {LedSwitcher::COLOR_GREEN,  LedSwitcher::MODE_BLINKING}}, // used by washing machine
  {"today",    TYPE_ALARM,  {LedSwitcher::COLOR_RED,    LedSwitcher::MODE_ON}},       // used by waste
  {"tomorrow", TYPE_ALARM,  {LedSwitcher::COLOR_YELLOW, LedSwitcher::MODE_ON}},       // used by waste
  {"none",     TYPE_ALARM,  {LedSwitcher::COLOR_OFF,    LedSwitcher::MODE_OFF}}       // used by waste
};

#endif
