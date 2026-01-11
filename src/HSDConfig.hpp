#pragma once

#include "HSDConfigFile.hpp"
#include "HSDEnums.hpp"
#include "HSDUtils.hpp"
#include <vector>

inline constexpr const char* jsonKeyHost = "host";
inline constexpr const char* jsonKeyWifiSsid = "wifiSSID";
inline constexpr const char* jsonKeyWifiPsk = "wifiPSK";
inline constexpr const char* jsonKeyMqttServer = "mqttServer";
inline constexpr const char* jsonKeyMqttUser = "mqttUser";
inline constexpr const char* jsonKeyMqttPassword = "mqttPassword";
inline constexpr const char* jsonKeyMqttStatusTopic = "mqttStatusTopic";
inline constexpr const char* jsonKeyMqttTestTopic = "mqttTestTopic";
inline constexpr const char* jsonKeyMqttWillTopic = "mqttWillTopic";
inline constexpr const char* jsonKeyLedCount = "ledCount";
inline constexpr const char* jsonKeyLedPin = "ledPin";
inline constexpr const char* jsonKeyLedType = "ledType";
inline constexpr const char* jsonKeyLedBrightness = "ledBrightness";
inline constexpr const char* jsonKeyColorMappingMsg = "m";
inline constexpr const char* jsonKeyColorMappingType = "t";
inline constexpr const char* jsonKeyColorMappingColor = "c";
inline constexpr const char* jsonKeyColorMappingBehavior = "b";
inline constexpr const char* jsonKeyDeviceMappingName = "n";
inline constexpr const char* jsonKeyDeviceMappingType = "t";
inline constexpr const char* jsonKeyDeviceMappingLed = "l";

class HSDConfig
{
  public:
    static constexpr int MaxDeviceMappingNameLen = 25;
    static constexpr int MaxColorMappingMsgLen = 15;

    struct ColorTranslator
    {
        Color color;
        uint32_t id;
    };

    struct DeviceMapping
    {
        DeviceMapping() = default;

        DeviceMapping(std::string_view n, DeviceType t, int l)
            : type(t)
            , ledNumber(l)
        {
            copyToArray(name, n);
        }

        std::array<char, MaxDeviceMappingNameLen> name{};
        DeviceType type = DeviceType::Unknown;
        int ledNumber = 0;
    };

    struct ColorMapping
    {
        ColorMapping() = default;

        ColorMapping(std::string_view m, DeviceType t, Color c, Behavior b)
            : type(t)
            , color(c)
            , behavior(b)
        {
            copyToArray(msg, m);
        }

        std::array<char, MaxColorMappingMsgLen> msg{};
        DeviceType type = DeviceType::Unknown;
        Color color = Color::None;
        Behavior behavior = Behavior::Off;
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
    bool setMqttServer(const char* server);

    const char* getMqttUser() const;
    bool setMqttUser(const char* user);

    const char* getMqttPassword() const;
    bool setMqttPassword(const char* password);

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

    bool addDeviceMappingEntry(size_t entryNum, std::string_view name, DeviceType type, int ledNumber);
    bool deleteColorMappingEntry(size_t entryNum);
    bool deleteAllDeviceMappingEntries();
    bool isDeviceMappingDirty() const;
    bool isDeviceMappingFull() const;

    bool addColorMappingEntry(size_t entryNum, std::string_view name, DeviceType type, Color color, Behavior behavior);
    bool deleteDeviceMappingEntry(size_t entryNum);
    bool deleteAllColorMappingEntries();
    bool isColorMappingDirty() const;
    bool isColorMappingFull() const;

    const DeviceMapping& getDeviceMapping(size_t index) const;
    const ColorMapping& getColorMapping(size_t index) const;
    int getLedNumber(std::string_view device, DeviceType type);
    std::optional<std::pair<std::string_view, DeviceType>> getDeviceInfo(int ledNumber);

    int getColorMapIndex(DeviceType deviceType, std::string_view msg);
    Behavior getLedBehavior(int colorMapIndex);
    Color getLedColor(int colorMapIndex);

    static uint32_t color2id(Color color)
    {
        auto it = std::find_if(colorTranslator.begin(), colorTranslator.end(),
                               [color](const auto& e)
                               { return e.color == color; });

        return (it != colorTranslator.end()) ? it->id : 0;
    }

    static Color id2color(uint32_t id)
    {
        auto it = std::find_if(colorTranslator.begin(), colorTranslator.end(),
                               [id](const auto& e)
                               { return e.id == id; });

        return (it != colorTranslator.end()) ? it->color : Color::None;
    }

  private:
    static constexpr std::array<ColorTranslator, 8> colorTranslator{{{Color::None, 0},
                                                                     {Color::Green, 1},
                                                                     {Color::Yellow, 2},
                                                                     {Color::Orange, 3},
                                                                     {Color::Red, 4},
                                                                     {Color::Purple, 5},
                                                                     {Color::Blue, 6},
                                                                     {Color::White, 7}}};

    bool readMainConfigFile();
    void printMainConfigFile(JsonObject& json);
    void writeMainConfigFile();

    bool readColorMappingConfigFile();
    void writeColorMappingConfigFile();

    bool readDeviceMappingConfigFile();
    void writeDeviceMappingConfigFile();

    void onFileWriteError();

    static constexpr size_t MaxVersionLen = 20;
    static constexpr size_t MaxHostLen = 30;
    static constexpr size_t MaxWifiSsidLen = 30;
    static constexpr size_t MaxWifiPskLen = 30;
    static constexpr size_t MaxMqttServerLen = 20;
    static constexpr size_t MaxMqttUserLen = 30;
    static constexpr size_t MaxMqttPasswordLen = 30;
    static constexpr size_t MaxMqttStatusTopicLen = 50;
    static constexpr size_t MaxMqttTestTopicLen = 50;
    static constexpr size_t MaxMqttWillTopicLen = 50;
    static constexpr size_t MaxColorMapEntries = 30;
    static constexpr size_t MaxDeviceMapEntries = 40;

    std::vector<ColorMapping> m_cfgColorMapping;
    bool m_cfgColorMappingDirty = false;

    std::vector<DeviceMapping> m_cfgDeviceMapping;
    bool m_cfgDeviceMappingDirty = false;

    std::array<char, MaxVersionLen + 1> m_cfgVersion{};
    std::array<char, MaxHostLen + 1> m_cfgHost{};
    std::array<char, MaxWifiSsidLen + 1> m_cfgWifiSSID{};
    std::array<char, MaxWifiPskLen + 1> m_cfgWifiPSK{};
    std::array<char, MaxMqttServerLen + 1> m_cfgMqttServer{};
    std::array<char, MaxMqttUserLen + 1> m_cfgMqttUser{};
    std::array<char, MaxMqttPasswordLen + 1> m_cfgMqttPassword{};
    std::array<char, MaxMqttStatusTopicLen + 1> m_cfgMqttStatusTopic{};
    std::array<char, MaxMqttTestTopicLen + 1> m_cfgMqttTestTopic{};
    std::array<char, MaxMqttWillTopicLen + 1> m_cfgMqttWillTopic{};
    int m_cfgNumberOfLeds = 0;
    int m_cfgLedDataPin = 0;
    int m_cfgLedType = 0;
    uint8_t m_cfgLedBrightness = 0;

    HSDConfigFile m_mainConfigFile;
    HSDConfigFile m_colorMappingConfigFile;
    HSDConfigFile m_deviceMappingConfigFile;
};
