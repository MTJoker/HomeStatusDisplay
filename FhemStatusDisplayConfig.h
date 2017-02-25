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
const char* MQTT_IN_TOPIC_TEST      = "fhem/cmd/statusdisplay_01/test";

/*
 * LED config
 */
const uint16_t NUMBER_OF_LEDS = 33;
const uint8_t LED_DATA_PIN = D2;

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
  {"open",     TYPE_WINDOW, Led::BLUE,   Led::ON}, 
  {"closed",   TYPE_WINDOW, Led::NONE,   Led::OFF}, 
  {"tilted",   TYPE_WINDOW, Led::YELLOW, Led::ON}, 
  {"open",     TYPE_DOOR,   Led::BLUE,   Led::ON}, 
  {"closed",   TYPE_DOOR,   Led::NONE,   Led::OFF}, 
  {"tilted",   TYPE_DOOR,   Led::YELLOW, Led::ON}, 
  {"on",       TYPE_LIGHT,  Led::GREEN,  Led::ON}, 
  {"off",      TYPE_LIGHT,  Led::NONE,   Led::OFF}, 
  {"true",     TYPE_ALARM,  Led::YELLOW, Led::FLASHING}, // used by oven, defectDevices, battery
  {"false",    TYPE_ALARM,  Led::NONE,   Led::OFF},      // used by oven, defectDevices, battery
  {"closed",   TYPE_ALARM,  Led::RED,    Led::BLINKING}, // used by water detectors
  {"open",     TYPE_ALARM,  Led::NONE,   Led::OFF},      // used by water detectors
  {"on",       TYPE_ALARM,  Led::GREEN,  Led::ON},       // used by washing machine
  {"off",      TYPE_ALARM,  Led::NONE,   Led::OFF},      // used by washing machine
  {"standby",  TYPE_ALARM,  Led::GREEN,  Led::BLINKING}, // used by washing machine
  {"today",    TYPE_ALARM,  Led::RED,    Led::ON},       // used by waste
  {"tomorrow", TYPE_ALARM,  Led::YELLOW, Led::ON},       // used by waste
  {"none",     TYPE_ALARM,  Led::NONE,   Led::OFF}       // used by waste
};

#endif
