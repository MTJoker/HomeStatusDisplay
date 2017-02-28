#pragma once

#include "FhemStatusDisplayTypes.h"
#include "Led.h"

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

  uint32_t getNumberOfLeds() const;
  bool setNumberOfLeds(uint32_t numberOfLeds);

  uint32_t getLedDataPin() const;
  bool setLedDataPin(uint32 dataPin);

  uint32_t getNumberOfColorMappingEntries() const;
  const colorMapping* getColorMapping(uint32_t index) const;

  bool addDeviceMappingEntry(String name, deviceType type, int ledNumber);
  bool addColorMappingEntry(String msg, deviceType type, Led::Color color, Led::Behavior behavior);

  int getLedNumber(String device, deviceType type);
  int getColorMapIndex(deviceType deviceType, String msg);
  Led::Behavior getLedBehavior(int colorMapIndex);
  Led::Color getLedColor(int colorMapIndex);

private:

  void resetMainConfigData();
  bool readMainConfigFile();
  void writeMainConfigFile();
  void createDefaultMainConfigFile();

  void resetColorMappingConfigData();
  bool readColorMappingConfigFile();
  void writeColorMappingConfigFile();
  void createDefaultColorMappingConfigFile();

  static const uint32_t MAX_VERSION_LEN           = 30;
  static const uint32_t MAX_HOST_LEN              = 30;
  static const uint32_t MAX_WIFI_SSID_LEN         = 40;
  static const uint32_t MAX_WIFI_PSK_LEN          = 40;
  static const uint32_t MAX_MQTT_SERVER_LEN       = 40;
  static const uint32_t MAX_MQTT_STATUS_TOPIC_LEN = 60;
  static const uint32_t MAX_MQTT_TEST_TOPIC_LEN   = 60;
  static const uint32_t MAX_MQTT_WILL_TOPIC_LEN   = 60;

  static const uint32_t MAX_COLOR_MAP_ENTRIES  = 50;
  static const uint32_t MAX_DEVICE_MAP_ENTRIES = 50;

  colorMapping m_cfgColorMapping[MAX_COLOR_MAP_ENTRIES];
  uint32_t m_numColorMappingEntries;
  
  deviceMapping m_cfgDeviceMapping[MAX_DEVICE_MAP_ENTRIES];
  uint32_t m_numDeviceMappingEntries;
  
  char m_cfgVersion[MAX_VERSION_LEN + 1];
  char m_cfgHost[MAX_HOST_LEN + 1];
  char m_cfgWifiSSID[MAX_WIFI_SSID_LEN + 1];
  char m_cfgWifiPSK[MAX_WIFI_PSK_LEN + 1];
  char m_cfgMqttServer[MAX_MQTT_SERVER_LEN + 1];
  char m_cfgMqttStatusTopic[MAX_MQTT_STATUS_TOPIC_LEN + 1];
  char m_cfgMqttTestTopic[MAX_MQTT_TEST_TOPIC_LEN + 1];
  char m_cfgMqttWillTopic[MAX_MQTT_WILL_TOPIC_LEN + 1];
  
  uint32_t m_cfgNumberOfLeds;
  uint32_t m_cfgLedDataPin;

};

