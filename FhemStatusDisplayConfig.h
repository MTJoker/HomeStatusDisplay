#pragma once

#include "FhemStatusDisplayTypes.h"
#include "Led.h"

#define JSON_KEY_HOST                  (F("host"))
#define JSON_KEY_WIFI_SSID             (F("wifiSSID"))
#define JSON_KEY_WIFI_PSK              (F("wifiPSK"))
#define JSON_KEY_MQTT_SERVER           (F("mqttServer"))
#define JSON_KEY_MQTT_STATUS_TOPIC     (F("mqttStatusTopic"))
#define JSON_KEY_MQTT_TEST_TOPIC       (F("mqttTestTopic"))
#define JSON_KEY_MQTT_WILL_TOPIC       (F("mqttWillTopic"))
#define JSON_KEY_LED_COUNT             (F("ledCount"))
#define JSON_KEY_LED_PIN               (F("ledPin"))
#define JSON_KEY_COLORMAPPING_MSG      (F("msg"))
#define JSON_KEY_COLORMAPPING_TYPE     (F("type"))
#define JSON_KEY_COLORMAPPING_COLOR    (F("color"))
#define JSON_KEY_COLORMAPPING_BEHAVIOR (F("behavior"))

class FhemStatusDisplayConfig
{
  
public:

  FhemStatusDisplayConfig();

  void begin(const char* version, const char* defaultIdentifier);

  void saveMain();
  void saveColorMapping();

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

  int getNumberOfColorMappingEntries() const;
  const colorMapping* getColorMapping(int index) const;

  bool addDeviceMappingEntry(String name, deviceType type, int ledNumber);
  
  bool addColorMappingEntry(String msg, deviceType type, Led::Color color, Led::Behavior behavior);
  void resetColorMappingConfigData();
    
  int getLedNumber(String device, deviceType type);
  int getColorMapIndex(deviceType deviceType, String msg);
  Led::Behavior getLedBehavior(int colorMapIndex);
  Led::Color getLedColor(int colorMapIndex);

private:

  void resetMainConfigData();
  bool readMainConfigFile();
  void writeMainConfigFile();
  void createDefaultMainConfigFile();

  bool readColorMappingConfigFile();
  void writeColorMappingConfigFile();
  void createDefaultColorMappingConfigFile();

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
};

