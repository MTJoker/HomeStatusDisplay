#pragma once

#include "HSDConfigFile.hpp"
#include "PreAllocatedLinkedList.hpp"

inline constexpr const char* JSON_KEY_HOST = "host";
inline constexpr const char* JSON_KEY_WIFI_SSID = "wifiSSID";
inline constexpr const char* JSON_KEY_WIFI_PSK = "wifiPSK";
inline constexpr const char* JSON_KEY_MQTT_SERVER = "mqttServer";
inline constexpr const char* JSON_KEY_MQTT_STATUS_TOPIC = "mqttStatusTopic";
inline constexpr const char* JSON_KEY_MQTT_TEST_TOPIC = "mqttTestTopic";
inline constexpr const char* JSON_KEY_MQTT_WILL_TOPIC = "mqttWillTopic";
inline constexpr const char* JSON_KEY_LED_COUNT = "ledCount";
inline constexpr const char* JSON_KEY_LED_PIN = "ledPin";
inline constexpr const char* JSON_KEY_LED_TYPE = "ledType";
inline constexpr const char* JSON_KEY_LED_BRIGHTNESS = "ledBrightness";
inline constexpr const char* JSON_KEY_COLORMAPPING_MSG = "m";
inline constexpr const char* JSON_KEY_COLORMAPPING_TYPE = "t";
inline constexpr const char* JSON_KEY_COLORMAPPING_COLOR = "c";
inline constexpr const char* JSON_KEY_COLORMAPPING_BEHAVIOR = "b";
inline constexpr const char* JSON_KEY_DEVICEMAPPING_NAME = "n";
inline constexpr const char* JSON_KEY_DEVICEMAPPING_TYPE = "t";
inline constexpr const char* JSON_KEY_DEVICEMAPPING_LED = "l";

class HSDConfig
{
  public:
    static constexpr int MAX_DEVICE_MAPPING_NAME_LEN = 25;
    static constexpr int MAX_COLOR_MAPPING_MSG_LEN = 15;

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
        TYPE_ALARM,
        TYPE_UNKNOWN
    };

    enum Behavior
    {
        OFF,
        ON,
        BLINKING,
        FLASHING,
        FLICKERING
    };

    enum Color
    {
        NONE = 0x000000,
        GREEN = 0x00FF00,
        YELLOW = 0xFFFF00,
        ORANGE = 0xFF5500,
        RED = 0xFF0000,
        PURPLE = 0xFF00FF,
        BLUE = 0x0000FF,
        WHITE = 0xFFFFFF
    };

    struct ColorTranslator
    {
        Color color;
        uint32_t id;
    };

    struct DeviceMapping
    {
        DeviceMapping()
            : type(TYPE_UNKNOWN)
            , ledNumber(0)
        {
            memset(name, 0, MAX_DEVICE_MAPPING_NAME_LEN);
        }

        DeviceMapping(const String& n, deviceType t, int l)
            : type(t)
            , ledNumber(l)
        {
            strncpy(name, n.c_str(), MAX_DEVICE_MAPPING_NAME_LEN - 1);
            name[MAX_DEVICE_MAPPING_NAME_LEN - 1] = '\0';
        }

        char name[MAX_DEVICE_MAPPING_NAME_LEN];
        deviceType type;
        int ledNumber;
    };

    struct ColorMapping
    {
        ColorMapping()
            : type(TYPE_UNKNOWN)
            , color(NONE)
            , behavior(OFF)
        {
            memset(msg, 0, MAX_COLOR_MAPPING_MSG_LEN + 1);
        }

        ColorMapping(const String& m, deviceType t, Color c, Behavior b)
            : type(t)
            , color(c)
            , behavior(b)
        {
            strncpy(msg, m.c_str(), MAX_COLOR_MAPPING_MSG_LEN);
            msg[MAX_COLOR_MAPPING_MSG_LEN] = '\0';
        }

        char msg[MAX_COLOR_MAPPING_MSG_LEN + 1];
        deviceType type;
        Color color;
        Behavior behavior;
    };

    HSDConfig();

    void begin(const char* version, const char* defaultIdentifier);

    void saveMain();
    void saveColorMapping();
    void updateColorMapping();
    void saveDeviceMapping();
    void updateDeviceMapping();
    bool createBackup(String& out) const;
    bool restoreBackup(const String& in);

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

    uint16_t getLedType() const;
    bool setLedType(uint16_t type);

    uint8_t getLedBrightness() const;
    bool setLedBrightness(uint8_t brightness);

    void resetMainConfigData();
    void resetDeviceMappingConfigData();
    void resetColorMappingConfigData();

    int getNumberOfDeviceMappingEntries() const;
    int getNumberOfColorMappingEntries();

    bool addDeviceMappingEntry(int entryNum, const String& name, deviceType type, int ledNumber);
    bool deleteColorMappingEntry(int entryNum);
    bool deleteAllDeviceMappingEntries();
    bool isDeviceMappingDirty() const;
    bool isDeviceMappingFull() const;

    bool addColorMappingEntry(int entryNum, const String& msg, deviceType type, Color color, Behavior behavior);
    bool deleteDeviceMappingEntry(int entryNum);
    bool deleteAllColorMappingEntries();
    bool isColorMappingDirty() const;
    bool isColorMappingFull() const;

    const DeviceMapping* getDeviceMapping(int index) const;
    const ColorMapping* getColorMapping(int index);
    int getLedNumber(const String& device, deviceType type);
    std::optional<std::pair<const char*, deviceType>> getDeviceInfo(int ledNumber);

    int getColorMapIndex(deviceType deviceType, const String& msg);
    Behavior getLedBehavior(int colorMapIndex);
    Color getLedColor(int colorMapIndex);

    static uint32_t color2id(Color color)
    {
        for(size_t i = 0; i < 8; i++)
        {
            if(colorTranslator[i].color == color)
                return colorTranslator[i].id;
        }
        return 0;
    }

    static Color id2color(uint32_t id)
    {
        for(size_t i = 0; i < 8; i++)
        {
            if(colorTranslator[i].id == id)
                return colorTranslator[i].color;
        }
        return NONE;
    }

  private:
    static inline constexpr ColorTranslator colorTranslator[8] =
        {
            {NONE, 0},
            {GREEN, 1},
            {YELLOW, 2},
            {ORANGE, 3},
            {RED, 4},
            {PURPLE, 5},
            {BLUE, 6},
            {WHITE, 7},
    };

    bool readMainConfigFile();
    void printMainConfigFile(JsonObject& json);
    void writeMainConfigFile();

    bool readColorMappingConfigFile();
    void writeColorMappingConfigFile();

    bool readDeviceMappingConfigFile();
    void writeDeviceMappingConfigFile();

    void onFileWriteError();

    static constexpr int MAX_VERSION_LEN = 20;
    static constexpr int MAX_HOST_LEN = 30;
    static constexpr int MAX_WIFI_SSID_LEN = 30;
    static constexpr int MAX_WIFI_PSK_LEN = 30;
    static constexpr int MAX_MQTT_SERVER_LEN = 20;
    static constexpr int MAX_MQTT_STATUS_TOPIC_LEN = 50;
    static constexpr int MAX_MQTT_TEST_TOPIC_LEN = 50;
    static constexpr int MAX_MQTT_WILL_TOPIC_LEN = 50;

    static constexpr int MAX_COLOR_MAP_ENTRIES = 30;
    static constexpr int MAX_DEVICE_MAP_ENTRIES = 35;

    PreAllocatedLinkedList<ColorMapping> m_cfgColorMapping;
    bool m_cfgColorMappingDirty = false;

    PreAllocatedLinkedList<DeviceMapping> m_cfgDeviceMapping;
    bool m_cfgDeviceMappingDirty = false;

    char m_cfgVersion[MAX_VERSION_LEN + 1]{};
    char m_cfgHost[MAX_HOST_LEN + 1]{};
    char m_cfgWifiSSID[MAX_WIFI_SSID_LEN + 1]{};
    char m_cfgWifiPSK[MAX_WIFI_PSK_LEN + 1]{};
    char m_cfgMqttServer[MAX_MQTT_SERVER_LEN + 1]{};
    char m_cfgMqttStatusTopic[MAX_MQTT_STATUS_TOPIC_LEN + 1]{};
    char m_cfgMqttTestTopic[MAX_MQTT_TEST_TOPIC_LEN + 1]{};
    char m_cfgMqttWillTopic[MAX_MQTT_WILL_TOPIC_LEN + 1]{};

    int m_cfgNumberOfLeds = 0;
    int m_cfgLedDataPin = 0;
    int m_cfgLedType = 0;
    uint8_t m_cfgLedBrightness = 0;

    HSDConfigFile m_mainConfigFile;
    HSDConfigFile m_colorMappingConfigFile;
    HSDConfigFile m_deviceMappingConfigFile;
};
