#include "HSDConfig.hpp"
#include <ArduinoJson.h>
#include <LittleFS.h>

static const int MAX_SIZE_MAIN_CONFIG_FILE = 400;
static const int JSON_BUFFER_MAIN_CONFIG_FILE = 500;

static const int MAX_SIZE_COLOR_MAPPING_CONFIG_FILE = 1500;    // 1401 exactly
static const int JSON_BUFFER_COLOR_MAPPING_CONFIG_FILE = 3800; // 3628 exactly

static const int MAX_SIZE_DEVICE_MAPPING_CONFIG_FILE = 1900;    // 1801 exactly
static const int JSON_BUFFER_DEVICE_MAPPING_CONFIG_FILE = 4000; // 3908 exactly

static const uint8_t DEFAULT_LED_BRIGHTNESS = 50;

HSDConfig::HSDConfig()
    : m_cfgColorMapping(MAX_COLOR_MAP_ENTRIES)
    , m_cfgDeviceMapping(MAX_DEVICE_MAP_ENTRIES)
    , m_mainConfigFile("/config.json")
    , m_colorMappingConfigFile("/colormapping.json")
    , m_deviceMappingConfigFile("/devicemapping.json")
{
    // reset non-configurable members
    setVersion("");
    setHost("");

    // reset configurable members
    resetMainConfigData();
    resetColorMappingConfigData();
    resetDeviceMappingConfigData();
}

void HSDConfig::begin(const char* version, const char* defaultIdentifier)
{
    Serial.println(F(""));
    Serial.println(F("Initializing config."));

    setVersion(version);
    setHost(defaultIdentifier);

    if(LittleFS.begin())
    {
        Serial.println(F("Mounted file system."));

        readMainConfigFile();
        readColorMappingConfigFile();
        readDeviceMappingConfigFile();
    }
    else
    {
        Serial.println(F("Failed to mount file system"));
    }
}

void HSDConfig::resetMainConfigData()
{
    Serial.println(F("Deleting main config data."));

    setWifiSSID("");
    setWifiPSK("");

    setMqttServer("");
    setMqttStatusTopic("");
    setMqttTestTopic("");
    setMqttWillTopic("");

    setNumberOfLeds(0);
    setLedDataPin(0);
    setLedBrightness(DEFAULT_LED_BRIGHTNESS);
}

void HSDConfig::resetColorMappingConfigData()
{
    Serial.println(F("Deleting color mapping config data."));
    m_cfgColorMapping.clear();
    m_cfgColorMappingDirty = true;
}

void HSDConfig::resetDeviceMappingConfigData()
{
    Serial.println(F("Deleting device mapping config data."));
    m_cfgDeviceMapping.clear();
    m_cfgDeviceMappingDirty = true;
}

bool HSDConfig::readMainConfigFile()
{
    bool success = false;
    char fileBuffer[MAX_SIZE_MAIN_CONFIG_FILE];

    if(m_mainConfigFile.read(fileBuffer, MAX_SIZE_MAIN_CONFIG_FILE))
    {
        DynamicJsonDocument doc(JSON_BUFFER_MAIN_CONFIG_FILE);
        DeserializationError error = deserializeJson(doc, fileBuffer);

        if(!error)
        {
            JsonObject json = doc.as<JsonObject>();

            Serial.println(F("Main config data successfully parsed."));
            Serial.print(F("JSON length is "));
            Serial.println(measureJson(json));

            printMainConfigFile(json);
            Serial.println(F(""));

            if(json.containsKey(JSON_KEY_HOST) &&
               json.containsKey(JSON_KEY_WIFI_SSID) &&
               json.containsKey(JSON_KEY_WIFI_PSK) &&
               json.containsKey(JSON_KEY_MQTT_SERVER) &&
               json.containsKey(JSON_KEY_MQTT_STATUS_TOPIC) &&
               json.containsKey(JSON_KEY_MQTT_TEST_TOPIC) &&
               json.containsKey(JSON_KEY_MQTT_WILL_TOPIC) &&
               json.containsKey(JSON_KEY_LED_COUNT) &&
               json.containsKey(JSON_KEY_LED_PIN))
            {
                Serial.println(F("Config data is complete."));

                setHost(json[JSON_KEY_HOST].as<const char*>());
                setWifiSSID(json[JSON_KEY_WIFI_SSID].as<const char*>());
                setWifiPSK(json[JSON_KEY_WIFI_PSK].as<const char*>());
                setMqttServer(json[JSON_KEY_MQTT_SERVER].as<const char*>());
                setMqttStatusTopic(json[JSON_KEY_MQTT_STATUS_TOPIC].as<const char*>());
                setMqttTestTopic(json[JSON_KEY_MQTT_TEST_TOPIC].as<const char*>());
                setMqttWillTopic(json[JSON_KEY_MQTT_WILL_TOPIC].as<const char*>());
                setNumberOfLeds(json[JSON_KEY_LED_COUNT].as<uint16_t>());
                setLedDataPin(json[JSON_KEY_LED_PIN].as<uint8_t>());

                uint8_t brightness = DEFAULT_LED_BRIGHTNESS;
                if(json.containsKey(JSON_KEY_LED_BRIGHTNESS))
                    brightness = json[JSON_KEY_LED_BRIGHTNESS].as<uint8_t>();
                setLedBrightness(brightness);

                success = true;
            }
        }
        else
        {
            Serial.print(F("Could not parse config data: "));
            Serial.println(error.c_str());
        }
    }
    else
    {
        Serial.println(F("Creating default main config file."));
        resetMainConfigData();
        writeMainConfigFile();
    }

    return success;
}

void HSDConfig::printMainConfigFile(JsonObject& json)
{
    Serial.print(F("  • host            : "));
    Serial.println(String(json[JSON_KEY_HOST]));
    Serial.print(F("  • wifiSSID        : "));
    Serial.println(String(json[JSON_KEY_WIFI_SSID]));
    Serial.println(F("  • wifiPSK         : not shown"));
    Serial.print(F("  • mqttServer      : "));
    Serial.println(String(json[JSON_KEY_MQTT_SERVER]));
    Serial.print(F("  • mqttStatusTopic : "));
    Serial.println(String(json[JSON_KEY_MQTT_STATUS_TOPIC]));
    Serial.print(F("  • mqttTestTopic   : "));
    Serial.println(String(json[JSON_KEY_MQTT_TEST_TOPIC]));
    Serial.print(F("  • mqttWillTopic   : "));
    Serial.println(String(json[JSON_KEY_MQTT_WILL_TOPIC]));
    Serial.print(F("  • ledCount        : "));
    Serial.println(json[JSON_KEY_LED_COUNT].as<int>());
    Serial.print(F("  • ledPin          : "));
    Serial.println(json[JSON_KEY_LED_PIN].as<int>());
    Serial.print(F("  • ledBrightness   : "));
    Serial.println(json[JSON_KEY_LED_BRIGHTNESS].as<int>());
}

bool HSDConfig::readColorMappingConfigFile()
{
    bool success = false;
    char fileBuffer[MAX_SIZE_COLOR_MAPPING_CONFIG_FILE];
    memset(fileBuffer, 0, MAX_SIZE_COLOR_MAPPING_CONFIG_FILE);
    resetColorMappingConfigData();

    if(m_colorMappingConfigFile.read(fileBuffer, MAX_SIZE_COLOR_MAPPING_CONFIG_FILE))
    {
        DynamicJsonDocument doc(JSON_BUFFER_COLOR_MAPPING_CONFIG_FILE);
        DeserializationError error = deserializeJson(doc, fileBuffer);

        if(!error)
        {
            JsonObject json = doc.as<JsonObject>();
            Serial.println(F("Color mapping config data successfully parsed."));
            Serial.print(F("JSON length is "));
            Serial.println(measureJson(json));
            Serial.println(F(""));

            success = true;
            size_t index = 0;

            for(JsonPair kv : json)
            {
                JsonObject entry = kv.value().as<JsonObject>();

                if(entry.containsKey(JSON_KEY_COLORMAPPING_MSG) &&
                   entry.containsKey(JSON_KEY_COLORMAPPING_TYPE) &&
                   entry.containsKey(JSON_KEY_COLORMAPPING_COLOR) &&
                   entry.containsKey(JSON_KEY_COLORMAPPING_BEHAVIOR))
                {
                    addColorMappingEntry(
                        index,
                        String(entry[JSON_KEY_COLORMAPPING_MSG]).c_str(),
                        (deviceType)(entry[JSON_KEY_COLORMAPPING_TYPE].as<int>()),
                        (Color)(id2color(entry[JSON_KEY_COLORMAPPING_COLOR].as<int>())),
                        (Behavior)(entry[JSON_KEY_COLORMAPPING_BEHAVIOR].as<int>()));

                    index++;
                }
            }
        }
        else
        {
            Serial.print(F("Could not parse config data: "));
            Serial.println(error.c_str());
        }
    }
    else
    {
        Serial.println(F("Creating default color mapping config file."));
        resetColorMappingConfigData();
        writeColorMappingConfigFile();
    }

    m_cfgColorMappingDirty = false;
    return success;
}

bool HSDConfig::readDeviceMappingConfigFile()
{
    bool success = false;
    char fileBuffer[MAX_SIZE_DEVICE_MAPPING_CONFIG_FILE];
    memset(fileBuffer, 0, MAX_SIZE_DEVICE_MAPPING_CONFIG_FILE);
    resetDeviceMappingConfigData();

    if(m_deviceMappingConfigFile.read(fileBuffer, MAX_SIZE_DEVICE_MAPPING_CONFIG_FILE))
    {
        DynamicJsonDocument doc(JSON_BUFFER_DEVICE_MAPPING_CONFIG_FILE);
        DeserializationError error = deserializeJson(doc, fileBuffer);

        if(!error)
        {
            JsonObject json = doc.as<JsonObject>();
            Serial.println(F("Device mapping config data successfully parsed."));
            Serial.print(F("JSON length is "));
            Serial.println(measureJson(json));
            Serial.println(F(""));

            success = true;
            size_t index = 0;

            for(JsonPair kv : json)
            {
                JsonObject entry = kv.value().as<JsonObject>();

                if(entry.containsKey(JSON_KEY_DEVICEMAPPING_NAME) &&
                   entry.containsKey(JSON_KEY_DEVICEMAPPING_TYPE) &&
                   entry.containsKey(JSON_KEY_DEVICEMAPPING_LED))
                {
                    addDeviceMappingEntry(
                        index,
                        String(entry[JSON_KEY_DEVICEMAPPING_NAME]).c_str(),
                        (deviceType)(entry[JSON_KEY_DEVICEMAPPING_TYPE].as<int>()),
                        entry[JSON_KEY_DEVICEMAPPING_LED].as<int>());

                    index++;
                }
            }
        }
        else
        {
            Serial.print(F("Could not parse config data: "));
            Serial.println(error.c_str());
        }
    }
    else
    {
        Serial.println(F("Creating default device mapping config file."));
        resetDeviceMappingConfigData();
        writeDeviceMappingConfigFile();
    }

    m_cfgDeviceMappingDirty = false;
    return success;
}

void HSDConfig::writeMainConfigFile()
{
    DynamicJsonDocument doc(JSON_BUFFER_MAIN_CONFIG_FILE);
    JsonObject json = doc.to<JsonObject>();

    json[JSON_KEY_HOST] = m_cfgHost;
    json[JSON_KEY_WIFI_SSID] = m_cfgWifiSSID;
    json[JSON_KEY_WIFI_PSK] = m_cfgWifiPSK;
    json[JSON_KEY_MQTT_SERVER] = m_cfgMqttServer;
    json[JSON_KEY_MQTT_STATUS_TOPIC] = m_cfgMqttStatusTopic;
    json[JSON_KEY_MQTT_TEST_TOPIC] = m_cfgMqttTestTopic;
    json[JSON_KEY_MQTT_WILL_TOPIC] = m_cfgMqttWillTopic;
    json[JSON_KEY_LED_COUNT] = m_cfgNumberOfLeds;
    json[JSON_KEY_LED_PIN] = m_cfgLedDataPin;
    json[JSON_KEY_LED_BRIGHTNESS] = m_cfgLedBrightness;

    if(!m_mainConfigFile.write(json))
    {
        onFileWriteError();
    }
}

void HSDConfig::writeColorMappingConfigFile()
{
    DynamicJsonDocument doc(JSON_BUFFER_COLOR_MAPPING_CONFIG_FILE);
    JsonObject json = doc.to<JsonObject>();

    for(size_t index = 0; index < m_cfgColorMapping.size(); index++)
    {
        const ColorMapping* mapping = m_cfgColorMapping.get(index);
        if(strlen(mapping->msg) != 0)
        {
            Serial.print(F("Preparing to write color mapping config file index "));
            Serial.print(String(index));
            Serial.print(F(", msg="));
            Serial.println(String(mapping->msg));
            JsonObject colorMappingEntry = json.createNestedObject(String(index));
            colorMappingEntry[JSON_KEY_COLORMAPPING_MSG] = mapping->msg;
            colorMappingEntry[JSON_KEY_COLORMAPPING_TYPE] = (int)mapping->type;
            colorMappingEntry[JSON_KEY_COLORMAPPING_COLOR] = (int)color2id(mapping->color);
            colorMappingEntry[JSON_KEY_COLORMAPPING_BEHAVIOR] = (int)mapping->behavior;
        }
        else
        {
            Serial.print(F("Removing color mapping config file index "));
            Serial.println(String(index));
        }
    }

    if(!m_colorMappingConfigFile.write(json))
    {
        onFileWriteError();
    }
    else
    {
        m_cfgColorMappingDirty = false;
    }
}

void HSDConfig::writeDeviceMappingConfigFile()
{
    DynamicJsonDocument doc(JSON_BUFFER_DEVICE_MAPPING_CONFIG_FILE);
    JsonObject json = doc.to<JsonObject>();

    for(size_t index = 0; index < m_cfgDeviceMapping.size(); index++)
    {
        const DeviceMapping* mapping = m_cfgDeviceMapping.get(index);
        if(strlen(mapping->name) != 0)
        {
            Serial.print(F("Preparing to write device mapping config file index "));
            Serial.println(String(index));
            JsonObject deviceMappingEntry = json.createNestedObject(String(index));
            deviceMappingEntry[JSON_KEY_DEVICEMAPPING_NAME] = mapping->name;
            deviceMappingEntry[JSON_KEY_DEVICEMAPPING_TYPE] = (int)mapping->type;
            deviceMappingEntry[JSON_KEY_DEVICEMAPPING_LED] = (int)mapping->ledNumber;
        }
        else
        {
            Serial.print(F("Removing device mapping config file index "));
            Serial.println(String(index));
        }
    }

    if(!m_deviceMappingConfigFile.write(json))
    {
        onFileWriteError();
    }
    else
    {
        m_cfgDeviceMappingDirty = false;
    }
}

void HSDConfig::saveMain()
{
    writeMainConfigFile();
}

void HSDConfig::saveColorMapping()
{
    writeColorMappingConfigFile();
}

void HSDConfig::updateColorMapping()
{
    readColorMappingConfigFile();
}

void HSDConfig::saveDeviceMapping()
{
    writeDeviceMappingConfigFile();
}

void HSDConfig::updateDeviceMapping()
{
    readDeviceMappingConfigFile();
}
bool HSDConfig::createBackup(String& out) const
{
    out = "{\"meta\":{\"format\":\"hsd-config-backup\",\"version\":1},";

    auto appendFile = [&](const char* key, const String& path, bool addComma) -> bool
    {
        File f = LittleFS.open(path, "r");
        if(!f)
        {
            Serial.print(F("Cannot open config file: "));
            Serial.println(path);
            return false;
        }

        out += "\"";
        out += key;
        out += "\":";

        out += f.readString();
        f.close();

        if(addComma)
        {
            out += ",";
        }

        return true;
    };

    if(!appendFile("config", m_mainConfigFile.getName(), true) ||
       !appendFile("colorMapping", m_colorMappingConfigFile.getName(), true) ||
       !appendFile("deviceMapping", m_deviceMappingConfigFile.getName(), false))
    {
        Serial.print(F("Cannot append config files to backup"));
        return false;
    }

    out += "}";
    return true;
}

bool HSDConfig::restoreBackup(const String& in)
{
    File f = LittleFS.open(in, "r");
    if(!f)
    {
        Serial.print(F("Cannot open backup file: "));
        Serial.println(in);
        return false;
    }

    String content = f.readString();
    f.close();

    DynamicJsonDocument doc(8192);
    DeserializationError err = deserializeJson(doc, content);
    if(err)
    {
        Serial.print(F("Restore parse error: "));
        Serial.println(err.c_str());
        return false;
    }

    auto writeConfigFile = [&](const String& path, const char* key) -> bool
    {
        File out = LittleFS.open(path, "w");
        if(!out)
        {
            Serial.print(F("Cannot open config file for writing: "));
            Serial.println(path);
            return false;
        }

        serializeJson(doc[key], out);
        out.close();

        return true;
    };

    if(!writeConfigFile(m_mainConfigFile.getName() + ".tmp", "config") ||
       !writeConfigFile(m_colorMappingConfigFile.getName() + ".tmp", "colorMapping") ||
       !writeConfigFile(m_deviceMappingConfigFile.getName() + ".tmp", "deviceMapping"))
    {
        Serial.print(F("Cannot write config files from backup"));
        return false;
    }

    LittleFS.remove(m_mainConfigFile.getName());
    LittleFS.remove(m_colorMappingConfigFile.getName());
    LittleFS.remove(m_deviceMappingConfigFile.getName());

    LittleFS.rename(m_mainConfigFile.getName() + ".tmp", m_mainConfigFile.getName());
    LittleFS.rename(m_colorMappingConfigFile.getName() + ".tmp", m_colorMappingConfigFile.getName());
    LittleFS.rename(m_deviceMappingConfigFile.getName() + ".tmp", m_deviceMappingConfigFile.getName());

    return true;
}

void HSDConfig::onFileWriteError()
{
    Serial.println(F("Failed to write file, formatting file system."));
    LittleFS.format();
    Serial.println(F("Done."));
}
bool HSDConfig::addDeviceMappingEntry(int entryNum, const String& name, deviceType type, int ledNumber)
{
    bool success = false;

    Serial.print(F("Adding or editing device mapping entry at index "));
    Serial.println(String(entryNum) + " with name " + name + ", type " +
                   String(type) + ", LED number " + String(ledNumber));

    DeviceMapping mapping(name, type, ledNumber);

    if(m_cfgDeviceMapping.set(entryNum, mapping))
    {
        m_cfgDeviceMappingDirty = true;
        success = true;
    }
    else
    {
        Serial.println(F("Cannot add/edit device mapping entry"));
    }

    return success;
}

bool HSDConfig::deleteDeviceMappingEntry(int entryNum)
{
    bool removed = m_cfgDeviceMapping.remove(entryNum);

    if(removed)
    {
        m_cfgDeviceMappingDirty = true;
    }

    return removed;
}

bool HSDConfig::deleteAllDeviceMappingEntries()
{
    m_cfgDeviceMapping.clear();
    m_cfgDeviceMappingDirty = true;
    return true;
}

bool HSDConfig::isDeviceMappingDirty() const
{
    return m_cfgDeviceMappingDirty;
}

bool HSDConfig::isDeviceMappingFull() const
{
    return m_cfgDeviceMapping.isFull();
}

bool HSDConfig::addColorMappingEntry(int entryNum, const String& msg, deviceType type, Color color, Behavior behavior)
{
    bool success = false;

    Serial.print(F("Adding or editing color mapping entry at index "));
    Serial.println(String(entryNum) + ", new values: name " + msg + ", type " +
                   String(type) + ", color " + String(color) + ", behavior " +
                   String(behavior));

    ColorMapping mapping(msg, type, color, behavior);

    if(m_cfgColorMapping.set(entryNum, mapping))
    {
        m_cfgColorMappingDirty = true;
        success = true;
    }
    else
    {
        Serial.println(F("Cannot add/edit device mapping entry"));
    }

    return success;
}

bool HSDConfig::deleteColorMappingEntry(int entryNum)
{
    bool removed = m_cfgColorMapping.remove(entryNum);

    if(removed)
    {
        m_cfgColorMappingDirty = true;
    }

    return removed;
}

bool HSDConfig::deleteAllColorMappingEntries()
{
    m_cfgColorMapping.clear();
    m_cfgColorMappingDirty = true;
    return true;
}

bool HSDConfig::isColorMappingDirty() const
{
    return m_cfgColorMappingDirty;
}

bool HSDConfig::isColorMappingFull() const
{
    return m_cfgColorMapping.isFull();
}

const char* HSDConfig::getHost() const
{
    return m_cfgHost;
}

bool HSDConfig::setHost(const char* host)
{
    strncpy(m_cfgHost, host, MAX_HOST_LEN);
    m_cfgHost[MAX_HOST_LEN] = '\0';
    return true;
}

const char* HSDConfig::getVersion() const
{
    return m_cfgVersion;
}

bool HSDConfig::setVersion(const char* version)
{
    strncpy(m_cfgVersion, version, MAX_VERSION_LEN);
    m_cfgVersion[MAX_VERSION_LEN] = '\0';
    return true;
}

const char* HSDConfig::getWifiSSID() const
{
    return m_cfgWifiSSID;
}

bool HSDConfig::setWifiSSID(const char* ssid)
{
    strncpy(m_cfgWifiSSID, ssid, MAX_WIFI_SSID_LEN);
    m_cfgWifiSSID[MAX_WIFI_SSID_LEN] = '\0';
    return true;
}

const char* HSDConfig::getWifiPSK() const
{
    return m_cfgWifiPSK;
}

bool HSDConfig::setWifiPSK(const char* psk)
{
    strncpy(m_cfgWifiPSK, psk, MAX_WIFI_PSK_LEN);
    m_cfgWifiPSK[MAX_WIFI_PSK_LEN] = '\0';
    return true;
}

const char* HSDConfig::getMqttServer() const
{
    return m_cfgMqttServer;
}

bool HSDConfig::setMqttServer(const char* ip)
{
    strncpy(m_cfgMqttServer, ip, MAX_MQTT_SERVER_LEN);
    m_cfgMqttServer[MAX_MQTT_SERVER_LEN] = '\0';
    return true;
}

const char* HSDConfig::getMqttStatusTopic() const
{
    return m_cfgMqttStatusTopic;
}

bool HSDConfig::setMqttStatusTopic(const char* topic)
{
    strncpy(m_cfgMqttStatusTopic, topic, MAX_MQTT_STATUS_TOPIC_LEN);
    m_cfgMqttStatusTopic[MAX_MQTT_STATUS_TOPIC_LEN] = '\0';
    return true;
}

const char* HSDConfig::getMqttTestTopic() const
{
    return m_cfgMqttTestTopic;
}

bool HSDConfig::setMqttTestTopic(const char* topic)
{
    strncpy(m_cfgMqttTestTopic, topic, MAX_MQTT_TEST_TOPIC_LEN);
    m_cfgMqttTestTopic[MAX_MQTT_TEST_TOPIC_LEN] = '\0';
    return true;
}

int HSDConfig::getNumberOfLeds() const
{
    return m_cfgNumberOfLeds;
}

const char* HSDConfig::getMqttWillTopic() const
{
    return m_cfgMqttWillTopic;
}

bool HSDConfig::setMqttWillTopic(const char* topic)
{
    strncpy(m_cfgMqttWillTopic, topic, MAX_MQTT_WILL_TOPIC_LEN);
    m_cfgMqttWillTopic[MAX_MQTT_WILL_TOPIC_LEN] = '\0';
    return true;
}

bool HSDConfig::setNumberOfLeds(uint32_t numberOfLeds)
{
    m_cfgNumberOfLeds = numberOfLeds;
    return true;
}

int HSDConfig::getLedDataPin() const
{
    return m_cfgLedDataPin;
}

bool HSDConfig::setLedDataPin(int dataPin)
{
    m_cfgLedDataPin = dataPin;
    return true;
}

uint8_t HSDConfig::getLedBrightness() const
{
    return m_cfgLedBrightness;
}

bool HSDConfig::setLedBrightness(uint8_t brightness)
{
    m_cfgLedBrightness = brightness;
    return true;
}

int HSDConfig::getNumberOfColorMappingEntries()
{
    return m_cfgColorMapping.size();
}

const HSDConfig::ColorMapping* HSDConfig::getColorMapping(int index)
{
    return m_cfgColorMapping.get(index);
}

int HSDConfig::getNumberOfDeviceMappingEntries() const
{
    return m_cfgDeviceMapping.size();
}

const HSDConfig::DeviceMapping* HSDConfig::getDeviceMapping(int index) const
{
    return m_cfgDeviceMapping.get(index);
}

int HSDConfig::getLedNumber(const String& deviceName, deviceType deviceType)
{
    int number = -1;

    for(size_t i = 0; i < m_cfgDeviceMapping.size(); i++)
    {
        const DeviceMapping* mapping = m_cfgDeviceMapping.get(i);

        if(deviceName.equals(mapping->name) && (deviceType == mapping->type))
        {
            number = mapping->ledNumber;
            break;
        }
    }

    return number;
}

std::optional<std::pair<const char*, HSDConfig::deviceType>> HSDConfig::getDeviceInfo(int ledNumber)
{
    for(size_t i = 0; i < m_cfgDeviceMapping.size(); i++)
    {
        const auto dm = m_cfgDeviceMapping.get(i);

        if(dm && ledNumber == dm->ledNumber)
        {
            return std::make_pair(dm->name, dm->type);
        }
    }

    return std::nullopt;
}

int HSDConfig::getColorMapIndex(deviceType deviceType, const String& msg)
{
    int index = -1;

    for(size_t i = 0; i < m_cfgColorMapping.size(); i++)
    {
        const ColorMapping* mapping = m_cfgColorMapping.get(i);

        if(msg.equals(mapping->msg) && (deviceType == mapping->type))
        {
            index = i;
            break;
        }
    }

    return index;
}

HSDConfig::Behavior HSDConfig::getLedBehavior(int colorMapIndex)
{
    return m_cfgColorMapping.get(colorMapIndex)->behavior;
}

HSDConfig::Color HSDConfig::getLedColor(int colorMapIndex)
{
    return m_cfgColorMapping.get(colorMapIndex)->color;
}
