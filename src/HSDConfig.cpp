#include "HSDConfig.hpp"
#include "HSDEnumsToString.hpp"
#include <ArduinoJson.h>
#include <LittleFS.h>

static constexpr int MaxSizeMainConfigFile = 400;
static constexpr int MaxSizeColorMappingConfigFile = 1500;
static constexpr int MaxSizeDeviceMappingConfigFile = 1900;
static constexpr const char* DefaultHostName = "HomeStatusDisplay";

HSDConfig::HSDConfig()
    : m_mainConfigFile("/config.json")
    , m_colorMappingConfigFile("/colormapping.json")
    , m_deviceMappingConfigFile("/devicemapping.json")
{
    m_cfgColorMapping.reserve(MaxColorMapEntries);
    m_cfgDeviceMapping.reserve(MaxDeviceMapEntries);

    Serial.print("m_cfgColorMapping.capacity() ? ");
    Serial.println(m_cfgColorMapping.capacity());

    Serial.print("m_cfgDeviceMapping.capacity() ? ");
    Serial.println(m_cfgDeviceMapping.capacity());

    resetMainConfigData();
    resetColorMappingConfigData();
    resetDeviceMappingConfigData();
}

void HSDConfig::begin()
{
    Serial.println(F(""));
    Serial.println(F("Initializing config."));

    setHost(DefaultHostName);

    if(LittleFS.begin())
    {
        Serial.println(F("Mounted file system."));

        m_mainConfigExists = readMainConfigFile();
        m_colorMappingConfigExists = readColorMappingConfigFile();
        m_deviceMappingConfigExists = readDeviceMappingConfigFile();
    }
    else
    {
        Serial.println(F("Failed to mount file system"));
    }
}

void HSDConfig::resetMainConfigData()
{
    Serial.println(F("Deleting main config data."));

    setHost("");
    setWifiSSID("");
    setWifiPSK("");

    setMqttServer("");
    setMqttUser("");
    setMqttPassword("");
    setMqttStatusTopic("");
    setMqttTestTopic("");
    setMqttWillTopic("");

    setNumberOfLeds(0);
    setLedDataPin(0);
    setLedType(0);
    setLedBrightness(0);
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

bool HSDConfig::hasWifiConfig() const
{
    return m_mainConfigExists && m_cfgWifiSSID[0] != '\0';
}

bool HSDConfig::hasMqttConfig() const
{
    return m_mainConfigExists && m_cfgMqttServer[0] != '\0';
}

bool HSDConfig::readMainConfigFile()
{
    bool success = false;
    char fileBuffer[MaxSizeMainConfigFile];

    if(m_mainConfigFile.read(fileBuffer, MaxSizeMainConfigFile))
    {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, fileBuffer);

        if(!error)
        {
            JsonObject json = doc.as<JsonObject>();

            Serial.println(F("Main config data successfully parsed."));
            Serial.print(F("JSON length is "));
            Serial.println(measureJson(json));

            printMainConfigFile(json);
            Serial.println(F(""));

            if(json[jsonKeyHost].is<const char*>() &&
               json[jsonKeyWifiSsid].is<const char*>() &&
               json[jsonKeyWifiPsk].is<const char*>() &&
               json[jsonKeyMqttServer].is<const char*>() &&
               json[jsonKeyMqttUser].is<const char*>() &&
               json[jsonKeyMqttPassword].is<const char*>() &&
               json[jsonKeyMqttStatusTopic].is<const char*>() &&
               json[jsonKeyMqttTestTopic].is<const char*>() &&
               json[jsonKeyMqttWillTopic].is<const char*>() &&
               json[jsonKeyLedCount].is<uint16_t>() &&
               json[jsonKeyLedPin].is<uint8_t>() &&
               json[jsonKeyLedType].is<uint16_t>() &&
               json[jsonKeyLedBrightness].is<uint8_t>())
            {
                Serial.println(F("Config data is complete."));

                setHost(json[jsonKeyHost].as<const char*>());
                setWifiSSID(json[jsonKeyWifiSsid].as<const char*>());
                setWifiPSK(json[jsonKeyWifiPsk].as<const char*>());
                setMqttServer(json[jsonKeyMqttServer].as<const char*>());
                setMqttUser(json[jsonKeyMqttUser].as<const char*>());
                setMqttPassword(json[jsonKeyMqttPassword].as<const char*>());
                setMqttStatusTopic(json[jsonKeyMqttStatusTopic].as<const char*>());
                setMqttTestTopic(json[jsonKeyMqttTestTopic].as<const char*>());
                setMqttWillTopic(json[jsonKeyMqttWillTopic].as<const char*>());

                setNumberOfLeds(json[jsonKeyLedCount].as<uint16_t>());
                setLedDataPin(json[jsonKeyLedPin].as<uint8_t>());
                setLedType(json[jsonKeyLedType].as<uint16_t>());
                setLedBrightness(json[jsonKeyLedBrightness].as<uint8_t>());

                success = true;
            }
            else
            {
                Serial.println(F("Config data is incomplete."));
            }
        }
        else
        {
            Serial.print(F("Could not parse config data: "));
            Serial.println(error.c_str());
        }
    }

    return success;
}

void HSDConfig::printMainConfigFile(JsonObject& json)
{
    Serial.print(F("  - host            : "));
    Serial.println(json[jsonKeyHost].as<const char*>());
    Serial.print(F("  - wifiSSID        : "));
    Serial.println(json[jsonKeyWifiSsid].as<const char*>());
    Serial.print(F("  - wifiPSK         : "));
    const char* wifiPwd = json[jsonKeyWifiPsk].as<const char*>();
    (wifiPwd != nullptr && wifiPwd[0] != '\0') ? Serial.println("not shown") : Serial.println();
    Serial.print(F("  - mqttServer      : "));
    Serial.println(json[jsonKeyMqttServer].as<const char*>());
    Serial.print(F("  - mqttUser        : "));
    Serial.println(json[jsonKeyMqttUser].as<const char*>());
    Serial.print(F("  - mqttPassword    : "));
    const char* mqttPwd = json[jsonKeyMqttPassword].as<const char*>();
    (mqttPwd != nullptr && mqttPwd[0] != '\0') ? Serial.println("not shown") : Serial.println();
    Serial.print(F("  - mqttStatusTopic : "));
    Serial.println(json[jsonKeyMqttStatusTopic].as<const char*>());
    Serial.print(F("  - mqttTestTopic   : "));
    Serial.println(json[jsonKeyMqttTestTopic].as<const char*>());
    Serial.print(F("  - mqttWillTopic   : "));
    Serial.println(json[jsonKeyMqttWillTopic].as<const char*>());
    Serial.print(F("  - ledCount        : "));
    Serial.println(json[jsonKeyLedCount].as<int>());
    Serial.print(F("  - ledPin          : "));
    Serial.println(json[jsonKeyLedPin].as<int>());
    Serial.print(F("  - ledType         : "));
    Serial.println(json[jsonKeyLedType].as<int>());
    Serial.print(F("  - ledBrightness   : "));
    Serial.println(json[jsonKeyLedBrightness].as<int>());
}

bool HSDConfig::readColorMappingConfigFile()
{
    bool success = false;
    char fileBuffer[MaxSizeColorMappingConfigFile];
    memset(fileBuffer, 0, MaxSizeColorMappingConfigFile);
    resetColorMappingConfigData();

    if(m_colorMappingConfigFile.read(fileBuffer, MaxSizeColorMappingConfigFile))
    {
        JsonDocument doc;
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

                if(entry[jsonKeyColorMappingMsg].is<const char*>() &&
                   entry[jsonKeyColorMappingType].is<int>() &&
                   entry[jsonKeyColorMappingColor].is<int>() &&
                   entry[jsonKeyColorMappingBehavior].is<int>())
                {
                    addColorMappingEntry(
                        index,
                        entry[jsonKeyColorMappingMsg].as<const char*>(),
                        static_cast<DeviceType>(entry[jsonKeyColorMappingType].as<int>()),
                        id2color(entry[jsonKeyColorMappingColor].as<int>()),
                        static_cast<Behavior>(entry[jsonKeyColorMappingBehavior].as<int>()));
                }

                index++;
            }
        }
        else
        {
            Serial.print(F("Could not parse config data: "));
            Serial.println(error.c_str());
        }
    }

    m_cfgColorMappingDirty = false;
    return success;
}

bool HSDConfig::readDeviceMappingConfigFile()
{
    bool success = false;
    char fileBuffer[MaxSizeDeviceMappingConfigFile];
    memset(fileBuffer, 0, MaxSizeDeviceMappingConfigFile);
    resetDeviceMappingConfigData();

    if(m_deviceMappingConfigFile.read(fileBuffer, MaxSizeDeviceMappingConfigFile))
    {
        JsonDocument doc;
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

                if(entry[jsonKeyDeviceMappingName].is<const char*>() &&
                   entry[jsonKeyDeviceMappingType].is<int>() &&
                   entry[jsonKeyDeviceMappingLed].is<int>())
                {
                    addDeviceMappingEntry(
                        index,
                        entry[jsonKeyDeviceMappingName].as<const char*>(),
                        static_cast<DeviceType>(entry[jsonKeyDeviceMappingType].as<int>()),
                        entry[jsonKeyDeviceMappingLed].as<int>());
                }

                index++;
            }
        }
        else
        {
            Serial.print(F("Could not parse config data: "));
            Serial.println(error.c_str());
        }
    }

    m_cfgDeviceMappingDirty = false;
    return success;
}

void HSDConfig::writeMainConfigFile()
{
    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>();

    json[jsonKeyHost] = m_cfgHost.data();
    json[jsonKeyWifiSsid] = m_cfgWifiSSID.data();
    json[jsonKeyWifiPsk] = m_cfgWifiPSK.data();
    json[jsonKeyMqttServer] = m_cfgMqttServer.data();
    json[jsonKeyMqttUser] = m_cfgMqttUser.data();
    json[jsonKeyMqttPassword] = m_cfgMqttPassword.data();
    json[jsonKeyMqttStatusTopic] = m_cfgMqttStatusTopic.data();
    json[jsonKeyMqttTestTopic] = m_cfgMqttTestTopic.data();
    json[jsonKeyMqttWillTopic] = m_cfgMqttWillTopic.data();
    json[jsonKeyLedCount] = m_cfgNumberOfLeds;
    json[jsonKeyLedPin] = m_cfgLedDataPin;
    json[jsonKeyLedType] = m_cfgLedType;
    json[jsonKeyLedBrightness] = m_cfgLedBrightness;

    if(!m_mainConfigFile.write(json))
    {
        onFileWriteError();
    }
}

void HSDConfig::writeColorMappingConfigFile()
{
    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>();

    for(size_t index = 0; index < m_cfgColorMapping.size(); index++)
    {
        auto mapping = m_cfgColorMapping[index];
        if(std::strlen(mapping.msg.data()) > 0)
        {
            Serial.print(F("Preparing to write color mapping config file index "));
            Serial.print(String(index));
            Serial.print(F(", msg="));
            Serial.println(String(mapping.msg.data()));

            JsonObject colorMappingEntry = json[String(index)].to<JsonObject>();
            colorMappingEntry[jsonKeyColorMappingMsg] = mapping.msg.data();
            colorMappingEntry[jsonKeyColorMappingType] = static_cast<int>(mapping.type);
            colorMappingEntry[jsonKeyColorMappingColor] = static_cast<int>(color2id(mapping.color));
            colorMappingEntry[jsonKeyColorMappingBehavior] = static_cast<int>(mapping.behavior);
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
    JsonDocument doc;
    JsonObject json = doc.to<JsonObject>();

    for(size_t index = 0; index < m_cfgDeviceMapping.size(); index++)
    {
        auto mapping = m_cfgDeviceMapping[index];
        if(std::strlen(mapping.name.data()) > 0)
        {
            Serial.print(F("Preparing to write device mapping config file index "));
            Serial.println(String(index));

            JsonObject deviceMappingEntry = json[String(index)].to<JsonObject>();
            deviceMappingEntry[jsonKeyDeviceMappingName] = mapping.name;
            deviceMappingEntry[jsonKeyDeviceMappingType] = static_cast<int>(mapping.type);
            deviceMappingEntry[jsonKeyDeviceMappingLed] = static_cast<int>(mapping.ledNumber);
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

    JsonDocument doc;
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

bool HSDConfig::addDeviceMappingEntry(size_t entryNum, std::string_view name, DeviceType type, int ledNumber)
{
    bool success = false;

    Serial.print(F("Adding or editing device mapping entry at index "));
    Serial.println(String(entryNum) + " with name " + name.data() + ", type " +
                   toString(type) + ", LED number " + String(ledNumber));

    if(entryNum < m_cfgDeviceMapping.capacity())
    {
        if(entryNum >= m_cfgDeviceMapping.size())
        {
            m_cfgDeviceMapping.resize(entryNum + 1);
        }
        m_cfgDeviceMapping[entryNum] = DeviceMapping(name, type, ledNumber);
        m_cfgDeviceMappingDirty = true;
        success = true;
    }
    else
    {
        Serial.println(F("Cannot add/edit device mapping entry"));
    }

    return success;
}

bool HSDConfig::deleteDeviceMappingEntry(size_t entryNum)
{
    if(entryNum >= m_cfgDeviceMapping.size())
    {
        return false;
    }

    m_cfgDeviceMapping.erase(m_cfgDeviceMapping.begin() + entryNum);
    m_cfgDeviceMappingDirty = true;

    return true;
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
    return (m_cfgDeviceMapping.size() >= m_cfgDeviceMapping.capacity());
}

bool HSDConfig::addColorMappingEntry(size_t entryNum, std::string_view msg, DeviceType type, Color color, Behavior behavior)
{
    bool success = false;

    Serial.print(F("Adding or editing color mapping entry at index "));
    Serial.println(String(entryNum) + ", new values: name " + msg.data() + ", type " +
                   toString(type) + ", color " + toString(color) + ", behavior " +
                   toString(behavior));

    if(entryNum < m_cfgColorMapping.capacity())
    {
        if(entryNum >= m_cfgColorMapping.size())
        {
            m_cfgColorMapping.resize(entryNum + 1);
        }
        m_cfgColorMapping[entryNum] = ColorMapping(msg, type, color, behavior);
        m_cfgColorMappingDirty = true;
        success = true;
    }
    else
    {
        Serial.println(F("Cannot add/edit device mapping entry"));
    }

    return success;
}

bool HSDConfig::deleteColorMappingEntry(size_t entryNum)
{
    if(entryNum >= m_cfgColorMapping.size())
    {
        return false;
    }

    m_cfgColorMapping.erase(m_cfgColorMapping.begin() + entryNum);
    m_cfgColorMappingDirty = true;

    return true;
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
    return (m_cfgColorMapping.size() >= m_cfgColorMapping.capacity());
}

const char* HSDConfig::getHost() const
{
    return m_cfgHost.data();
}

bool HSDConfig::setHost(const char* host)
{
    copyToArray(m_cfgHost, host);
    return true;
}

const char* HSDConfig::getWifiSSID() const
{
    return m_cfgWifiSSID.data();
}

bool HSDConfig::setWifiSSID(const char* ssid)
{
    copyToArray(m_cfgWifiSSID, ssid);
    return true;
}

const char* HSDConfig::getWifiPSK() const
{
    return m_cfgWifiPSK.data();
}

bool HSDConfig::setWifiPSK(const char* psk)
{
    copyToArray(m_cfgWifiPSK, psk);
    return true;
}

const char* HSDConfig::getMqttServer() const
{
    return m_cfgMqttServer.data();
}

bool HSDConfig::setMqttServer(const char* server)
{
    copyToArray(m_cfgMqttServer, server);
    return true;
}

const char* HSDConfig::getMqttUser() const
{
    return m_cfgMqttUser.data();
}

bool HSDConfig::setMqttUser(const char* user)
{
    copyToArray(m_cfgMqttUser, user);
    return true;
}

const char* HSDConfig::getMqttPassword() const
{
    return m_cfgMqttPassword.data();
}

bool HSDConfig::setMqttPassword(const char* password)
{
    copyToArray(m_cfgMqttPassword, password);
    return true;
}

const char* HSDConfig::getMqttStatusTopic() const
{
    return m_cfgMqttStatusTopic.data();
}

bool HSDConfig::setMqttStatusTopic(const char* topic)
{
    copyToArray(m_cfgMqttStatusTopic, topic);
    return true;
}

const char* HSDConfig::getMqttTestTopic() const
{
    return m_cfgMqttTestTopic.data();
}

bool HSDConfig::setMqttTestTopic(const char* topic)
{
    copyToArray(m_cfgMqttTestTopic, topic);
    return true;
}

int HSDConfig::getNumberOfLeds() const
{
    return m_cfgNumberOfLeds;
}

const char* HSDConfig::getMqttWillTopic() const
{
    return m_cfgMqttWillTopic.data();
}

bool HSDConfig::setMqttWillTopic(const char* topic)
{
    copyToArray(m_cfgMqttWillTopic, topic);
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

uint16_t HSDConfig::getLedType() const
{
    return m_cfgLedType;
}

bool HSDConfig::setLedType(uint16_t type)
{
    m_cfgLedType = type;
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

const HSDConfig::ColorMapping& HSDConfig::getColorMapping(size_t index) const
{
    return m_cfgColorMapping[index];
}

int HSDConfig::getNumberOfDeviceMappingEntries() const
{
    return m_cfgDeviceMapping.size();
}

const HSDConfig::DeviceMapping& HSDConfig::getDeviceMapping(size_t index) const
{
    return m_cfgDeviceMapping[index];
}

int HSDConfig::getLedNumber(std::string_view device, DeviceType deviceType)
{
    int number = -1;

    for(const auto& mapping : m_cfgDeviceMapping)
    {
        if(deviceType == mapping.type &&
           std::strcmp(mapping.name.data(), device.data()) == 0)
        {
            number = mapping.ledNumber;
            break;
        }
    }

    return number;
}

std::optional<std::pair<std::string_view, DeviceType>> HSDConfig::getDeviceInfo(int ledNumber)
{
    for(const auto& mapping : m_cfgDeviceMapping)
    {
        if(ledNumber == mapping.ledNumber)
        {
            Serial.print("DEBUG: Name = ");
            Serial.println(mapping.name.data());
            return std::make_pair(std::string_view(mapping.name.data()), mapping.type);
        }
    }

    return std::nullopt;
}

int HSDConfig::getColorMapIndex(DeviceType deviceType, std::string_view msg)
{
    int index = -1;

    for(size_t i = 0; i < m_cfgColorMapping.size(); i++)
    {
        auto mapping = m_cfgColorMapping[i];

        if(deviceType == mapping.type && std::strcmp(mapping.msg.data(), msg.data()) == 0)
        {
            index = i;
            break;
        }
    }

    return index;
}

Behavior HSDConfig::getLedBehavior(int colorMapIndex)
{
    return m_cfgColorMapping[colorMapIndex].behavior;
}

Color HSDConfig::getLedColor(int colorMapIndex)
{
    return m_cfgColorMapping[colorMapIndex].color;
}
