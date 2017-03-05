#pragma once

#include "HSDLed.h"
#include "HSDConfigFile.h"

#define JSON_KEY_HOST                  (F("host"))
#define JSON_KEY_WIFI_SSID             (F("wifiSSID"))
#define JSON_KEY_WIFI_PSK              (F("wifiPSK"))
#define JSON_KEY_MQTT_SERVER           (F("mqttServer"))
#define JSON_KEY_MQTT_STATUS_TOPIC     (F("mqttStatusTopic"))
#define JSON_KEY_MQTT_TEST_TOPIC       (F("mqttTestTopic"))
#define JSON_KEY_MQTT_WILL_TOPIC       (F("mqttWillTopic"))
#define JSON_KEY_LED_COUNT             (F("ledCount"))
#define JSON_KEY_LED_PIN               (F("ledPin"))
#define JSON_KEY_COLORMAPPING_OIU      (F("oiu"))
#define JSON_KEY_COLORMAPPING_MSG      (F("msg"))
#define JSON_KEY_COLORMAPPING_TYPE     (F("type"))
#define JSON_KEY_COLORMAPPING_COLOR    (F("color"))
#define JSON_KEY_COLORMAPPING_BEHAVIOR (F("behavior"))
#define JSON_KEY_DEVICEMAPPING_NAME    (F("name"))
#define JSON_KEY_DEVICEMAPPING_TYPE    (F("type"))
#define JSON_KEY_DEVICEMAPPING_LED     (F("led"))

class HSDConfig
{
  
public:

  static const int MAX_DEVICE_MAPPING_NAME_LEN = 30;
  static const int MAX_COLOR_MAPPING_MSG_LEN = 20;
  
  /*
   * Enum which defines the types of devices which can send messages.
   * If the same message (e.g. "on") can be received from different types
   * of devices (e.g. light and alarm), different reaction can be done.
   */
  enum deviceType
  {
    TYPE_WINDOW,
    TYPE_DOOR,
    TYPE_LIGHT,
    TYPE_ALARM
  };

  /*
   * This struct is used for mapping a device of a specific device type 
   * to a led number, that means a specific position on the led stripe
   */
  struct deviceMapping
  {
    char name[MAX_DEVICE_MAPPING_NAME_LEN]; // name of the device
    deviceType type;                        // type of the device
    int ledNumber;                          // led number on which reactions for this device are displayed
  };
  
  /*
   * This struct is used for mapping a message for a specific device
   * type to a led behavior (see LedSwitcher::ledState).
   */
  struct colorMapping
  {
    char msg[MAX_COLOR_MAPPING_MSG_LEN+1];  // message 
    deviceType type;                        // type of the device
    HSDLed::Color color;                    // led color for message from device type
    HSDLed::Behavior behavior;              // led behavior for message from device type
  };

  HSDConfig();

  void begin(const char* version, const char* defaultIdentifier);

  void saveMain();
  void saveColorMapping();
  void saveDeviceMapping();

  const char* getVersion() const;
  bool setVersion(const char* version);

  const char* getHost() const;
  bool setHost(const char* host);

  const char* getWifiSSID() const;
  bool setWifiSSID(const char* ssid);

  const char* getWifiPSK() const;
  bool setWifiPSK(const char* psk);

  const char* getMqttServer() const;
  bool setMqttServer(const char* ip);

  const char* getMqttStatusTopic() const;
  bool setMqttStatusTopic(const char* topic);

  const char* getMqttTestTopic() const;
  bool setMqttTestTopic(const char* topic);

  const char* getMqttWillTopic() const;
  bool setMqttWillTopic(const char* topic);

  int getNumberOfLeds() const;
  bool setNumberOfLeds(uint32_t numberOfLeds);

  int getLedDataPin() const;
  bool setLedDataPin(int dataPin);

  void resetMainConfigData();
  void resetDeviceMappingConfigData();
  void resetColorMappingConfigData();

  int getNumberOfDeviceMappingEntries() const;
  int getNumberOfColorMappingEntries() const;
  
  bool addDeviceMappingEntry(String name, deviceType type, int ledNumber);
  bool addColorMappingEntry(String msg, deviceType type, HSDLed::Color color, HSDLed::Behavior behavior);

  const deviceMapping* getDeviceMapping(int index) const;
  const colorMapping* getColorMapping(int index) const; 

  void setSwitchLedOffIfUnknownMessage(bool switchOff);
  bool isSwitchLedOffIfUnknownMessage() const;
    
  int getLedNumber(String device, deviceType type);
  int getColorMapIndex(deviceType deviceType, String msg);
  HSDLed::Behavior getLedBehavior(int colorMapIndex);
  HSDLed::Color getLedColor(int colorMapIndex);

private:

  bool readMainConfigFile();
  void writeMainConfigFile();

  bool readColorMappingConfigFile();
  void writeColorMappingConfigFile();

  bool readDeviceMappingConfigFile();
  void writeDeviceMappingConfigFile();

  static const int MAX_VERSION_LEN           = 20;
  static const int MAX_HOST_LEN              = 30;
  static const int MAX_WIFI_SSID_LEN         = 30;
  static const int MAX_WIFI_PSK_LEN          = 30;
  static const int MAX_MQTT_SERVER_LEN       = 20;
  static const int MAX_MQTT_STATUS_TOPIC_LEN = 50;
  static const int MAX_MQTT_TEST_TOPIC_LEN   = 50;
  static const int MAX_MQTT_WILL_TOPIC_LEN   = 50;

  static const int MAX_COLOR_MAP_ENTRIES  = 50;
  static const int MAX_DEVICE_MAP_ENTRIES = 50;

  colorMapping m_cfgColorMapping[MAX_COLOR_MAP_ENTRIES];
  int m_numColorMappingEntries;
  bool m_cfgSwitchLedOffIfUnknownMessage;
  
  deviceMapping m_cfgDeviceMapping[MAX_DEVICE_MAP_ENTRIES];
  int m_numDeviceMappingEntries;
  
  char m_cfgVersion[MAX_VERSION_LEN + 1];
  char m_cfgHost[MAX_HOST_LEN + 1];
  char m_cfgWifiSSID[MAX_WIFI_SSID_LEN + 1];
  char m_cfgWifiPSK[MAX_WIFI_PSK_LEN + 1];
  char m_cfgMqttServer[MAX_MQTT_SERVER_LEN + 1];
  char m_cfgMqttStatusTopic[MAX_MQTT_STATUS_TOPIC_LEN + 1];
  char m_cfgMqttTestTopic[MAX_MQTT_TEST_TOPIC_LEN + 1];
  char m_cfgMqttWillTopic[MAX_MQTT_WILL_TOPIC_LEN + 1];
  
  int m_cfgNumberOfLeds;
  int m_cfgLedDataPin;

  HSDConfigFile m_mainConfigFile;
  HSDConfigFile m_colorMappingConfigFile;
  HSDConfigFile m_deviceMappingConfigFile;
};

