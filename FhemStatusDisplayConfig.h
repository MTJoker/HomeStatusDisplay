#pragma once

#include "FhemStatusDisplayTypes.h"
#include "Led.h"

static const deviceMapping deviceMap[] =
{
  // 1st row
  {"basement",   TYPE_DOOR,    0},
  {"main",       TYPE_DOOR,    1},
  {"kitchen",    TYPE_WINDOW,  2},
  {"eating",     TYPE_WINDOW,  3},
  {"terrace",    TYPE_DOOR,    4},
  {"bath_left",  TYPE_WINDOW,  5},
  {"bath_right", TYPE_WINDOW,  6},
  {"child",      TYPE_WINDOW,  7},
  {"sleep",      TYPE_WINDOW,  8},
  {"work",       TYPE_WINDOW,  9},
  {"garage",     TYPE_DOOR,   10},

  //2nd row
  {"kitchen_ceiling_right", TYPE_LIGHT, 11},
  {"kitchen_ceiling_left",  TYPE_LIGHT, 12},
  {"eating_ceiling",        TYPE_LIGHT, 13},
  {"living_ceiling",        TYPE_LIGHT, 14},
  {"living_stonewall",      TYPE_LIGHT, 15},
  {"living_lowboard",       TYPE_LIGHT, 16},
  {"living_shelf",          TYPE_LIGHT, 17},
  {"bath_ceiling",          TYPE_LIGHT, 18},
  {"child_ceiling",         TYPE_LIGHT, 19},
  {"sleep_ceiling",         TYPE_LIGHT, 20},
  {"work_ceiling",          TYPE_LIGHT, 21},

  //3rd row
  {"washing_machine",    TYPE_ALARM, 22},
  {"waterdetector_1",    TYPE_ALARM, 23},
  {"waterdetector_2",    TYPE_ALARM, 24},
  {"oven",               TYPE_ALARM, 25},
  {"waste_residual_bio", TYPE_ALARM, 26},
  {"waste_paper_yellow", TYPE_ALARM, 27},
  {"device_error",       TYPE_ALARM, 28},
  {"battery_error",      TYPE_ALARM, 29},
  {"unused_3",           TYPE_ALARM, 30},
  {"unused_2",           TYPE_ALARM, 31},
  {"unused_1",           TYPE_ALARM, 32},
};

static const colorMapping colorMap[] = 
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

class FhemStatusDisplayConfig
{
  
public:

  FhemStatusDisplayConfig();

  void begin();

  const String getWifiSSID() const;
  bool setWifiSSID(String ssid);

  const String getWifiPSK() const;
  bool setWifiPSK(String psk);

  const char* getMqttServer() const;
  bool setMqttServer(const char* ip);

  const char* getMqttStatusTopic() const;
  bool setMqttStatusTopic(const char* topic);

  const char* getMqttTestTopic() const;
  bool setMqttTestTopic(const char* topic);

  uint32_t getNumberOfLeds() const;
  void setNumberOfLeds(uint32_t numberOfLeds);

  uint32_t getLedDataPin() const;
  void setLedDataPin(uint32 dataPin);

  int getLedNumber(String device, deviceType type);
  int getColorMapIndex(deviceType deviceType, String msg);
  Led::Behavior getLedBehavior(int colorMapIndex);
  Led::Color getLedColor(int colorMapIndex);

private:

  char m_cfgWifiSSID[40];
  char m_cfgWifiPSK[40];
  char m_cfgMqttServer[40];
  char m_cfgMqttStatusTopic[100];
  char m_cfgMqttTestTopic[100];
  uint32_t m_cfgNumberOfLeds;
  uint32_t m_cfgLedDataPin;

};

