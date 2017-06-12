#include "HSDConfig.h"
#include <FS.h>
#include <ArduinoJson.h>

static const int MAX_SIZE_MAIN_CONFIG_FILE = 400;
static const int JSON_BUFFER_MAIN_CONFIG_FILE = 500;

static const int MAX_SIZE_COLOR_MAPPING_CONFIG_FILE = 1800;    //1611 exactly
static const int JSON_BUFFER_COLOR_MAPPING_CONFIG_FILE = 4000; //3838 exactly

static const int MAX_SIZE_DEVICE_MAPPING_CONFIG_FILE = 1300;
static const int JSON_BUFFER_DEVICE_MAPPING_CONFIG_FILE = 3100;

static const uint8_t DEFAULT_LED_BRIGHTNESS = 50;
 
HSDConfig::HSDConfig()
:
m_mainConfigFile(String("/config.json")),
m_colorMappingConfigFile(String("/colormapping.json")),
m_deviceMappingConfigFile(String("/devicemapping.json"))
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
  
  if(SPIFFS.begin())
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
  
  memset(m_cfgColorMapping, 0, sizeof(m_cfgColorMapping));
  m_numColorMappingEntries = 0;
}

void HSDConfig::resetDeviceMappingConfigData()
{
  Serial.println(F("Deleting device mapping config data."));
  
  memset(m_cfgDeviceMapping, 0, sizeof(m_cfgDeviceMapping));
  m_numDeviceMappingEntries = 0;  
}

bool HSDConfig::readMainConfigFile()
{
  bool success = false;

  char fileBuffer[MAX_SIZE_MAIN_CONFIG_FILE];

  if(m_mainConfigFile.read(fileBuffer, MAX_SIZE_MAIN_CONFIG_FILE))
  {
    DynamicJsonBuffer jsonBuffer(JSON_BUFFER_MAIN_CONFIG_FILE);
    JsonObject& json = jsonBuffer.parseObject(fileBuffer);

    if (json.success()) 
    {
      Serial.println(F("Main config data successfully parsed."));
      Serial.print(F("JSON length is ")); Serial.println(json.measureLength());     
      json.prettyPrintTo(Serial);
      Serial.println(F(""));

      if(json.containsKey(JSON_KEY_HOST) && json.containsKey(JSON_KEY_WIFI_SSID) && json.containsKey(JSON_KEY_WIFI_PSK) && 
         json.containsKey(JSON_KEY_MQTT_SERVER) && json.containsKey(JSON_KEY_MQTT_STATUS_TOPIC) && json.containsKey(JSON_KEY_MQTT_TEST_TOPIC) && json.containsKey(JSON_KEY_MQTT_WILL_TOPIC) &&
         json.containsKey(JSON_KEY_LED_COUNT) && json.containsKey(JSON_KEY_LED_PIN))
      {
        Serial.println(F("Config data is complete."));

        setHost(json[JSON_KEY_HOST]);
        setWifiSSID(json[JSON_KEY_WIFI_SSID]);
        setWifiPSK(json[JSON_KEY_WIFI_PSK]);
        setMqttServer(json[JSON_KEY_MQTT_SERVER]);
        setMqttStatusTopic(json[JSON_KEY_MQTT_STATUS_TOPIC]);
        setMqttTestTopic(json[JSON_KEY_MQTT_TEST_TOPIC]);
        setMqttWillTopic(json[JSON_KEY_MQTT_WILL_TOPIC]);
        setNumberOfLeds(json[JSON_KEY_LED_COUNT]);
        setLedDataPin(json[JSON_KEY_LED_PIN]);
        setLedBrightness(json[JSON_KEY_LED_BRIGHTNESS]);

        success = true;
      }
    } 
    else 
    {
      Serial.println(F("Could not parse config data."));
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

bool HSDConfig::readColorMappingConfigFile()
{
  bool success = false;

  char fileBuffer[MAX_SIZE_COLOR_MAPPING_CONFIG_FILE];
  memset(fileBuffer, 0, MAX_SIZE_COLOR_MAPPING_CONFIG_FILE);
  resetColorMappingConfigData();

  if(m_colorMappingConfigFile.read(fileBuffer, MAX_SIZE_COLOR_MAPPING_CONFIG_FILE))
  {
    DynamicJsonBuffer jsonBuffer(JSON_BUFFER_COLOR_MAPPING_CONFIG_FILE);
    JsonObject& json = jsonBuffer.parseObject(fileBuffer);

    if (json.success()) 
    {
      Serial.println(F("Color mapping config data successfully parsed."));
      Serial.print(F("JSON length is ")); Serial.println(json.measureLength());  
      json.prettyPrintTo(Serial);
      Serial.println(F(""));

      success = true;
      int index = 0;
      
      for(JsonObject::iterator it = json.begin(); it != json.end(); ++it)
      {
        JsonObject& entry = json[it->key]; 

        if(entry.containsKey(JSON_KEY_COLORMAPPING_MSG) && entry.containsKey(JSON_KEY_COLORMAPPING_TYPE) &&
           entry.containsKey(JSON_KEY_COLORMAPPING_COLOR) && entry.containsKey(JSON_KEY_COLORMAPPING_BEHAVIOR) )
        {
          addColorMappingEntry(index,
                               entry[JSON_KEY_COLORMAPPING_MSG].as<char*>(), 
                               (deviceType)(entry[JSON_KEY_COLORMAPPING_TYPE].as<int>()), 
                               (Color)(entry[JSON_KEY_COLORMAPPING_COLOR].as<int>()), 
                               (Behavior)(entry[JSON_KEY_COLORMAPPING_BEHAVIOR].as<int>())); 

          index++;
        }
      }
    }
    else
    {
      Serial.println(F("Could not parse config data."));
    }
  }
  else
  {
    Serial.println(F("Creating default color mapping config file."));
    resetColorMappingConfigData();
    writeColorMappingConfigFile();
  }
}

bool HSDConfig::readDeviceMappingConfigFile()
{
  bool success = false;

  char fileBuffer[MAX_SIZE_DEVICE_MAPPING_CONFIG_FILE];
  memset(fileBuffer, 0, MAX_SIZE_DEVICE_MAPPING_CONFIG_FILE);
  resetDeviceMappingConfigData();

  if(m_deviceMappingConfigFile.read(fileBuffer, MAX_SIZE_DEVICE_MAPPING_CONFIG_FILE))
  {
    DynamicJsonBuffer jsonBuffer(JSON_BUFFER_DEVICE_MAPPING_CONFIG_FILE);
    JsonObject& json = jsonBuffer.parseObject(fileBuffer);

    if (json.success()) 
    {
      Serial.println(F("Device mapping config data successfully parsed."));
      Serial.print(F("JSON length is ")); Serial.println(json.measureLength());  
      json.prettyPrintTo(Serial);
      Serial.println(F(""));

      success = true;
      
      for(JsonObject::iterator it = json.begin(); it != json.end(); ++it)
      {
        JsonObject& entry = json[it->key]; 

        if(entry.containsKey(JSON_KEY_DEVICEMAPPING_NAME) && entry.containsKey(JSON_KEY_DEVICEMAPPING_TYPE) &&
           entry.containsKey(JSON_KEY_DEVICEMAPPING_LED) )
        {
          addDeviceMappingEntry(entry[JSON_KEY_DEVICEMAPPING_NAME].as<char*>(), 
                               (deviceType)(entry[JSON_KEY_DEVICEMAPPING_TYPE].as<int>()), 
                               entry[JSON_KEY_DEVICEMAPPING_LED].as<int>()); 
        }
      }
    }
    else
    {
      Serial.println(F("Could not parse config data."));
    }
  }
  else
  {
    Serial.println(F("Creating default device mapping config file."));
    resetDeviceMappingConfigData();
    writeDeviceMappingConfigFile();
  }
}

void HSDConfig::writeMainConfigFile()
{
  DynamicJsonBuffer jsonBuffer(JSON_BUFFER_MAIN_CONFIG_FILE);
  JsonObject& json = jsonBuffer.createObject();

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

  if(!m_mainConfigFile.write(&json))
  {
    onFileWriteError();
  }
}

void HSDConfig::writeColorMappingConfigFile()
{
  DynamicJsonBuffer jsonBuffer(JSON_BUFFER_COLOR_MAPPING_CONFIG_FILE);
  JsonObject& json = jsonBuffer.createObject();

  int numEntries = 0; // needed for skipping empty entries
    
  for(int index = 0; index < m_numColorMappingEntries; index++)
  { 
    if(strlen(m_cfgColorMapping[index].msg) != 0)
    {
      Serial.print(F("Preparing to write color mapping config file index "));
      Serial.println(String(numEntries));
      
      JsonObject& colorMappingEntry = json.createNestedObject(String(numEntries));
  
      colorMappingEntry[JSON_KEY_COLORMAPPING_MSG] = m_cfgColorMapping[index].msg ;
      colorMappingEntry[JSON_KEY_COLORMAPPING_TYPE] = (int)m_cfgColorMapping[index].type;
      colorMappingEntry[JSON_KEY_COLORMAPPING_COLOR] = (int)m_cfgColorMapping[index].color;
      colorMappingEntry[JSON_KEY_COLORMAPPING_BEHAVIOR] = (int)m_cfgColorMapping[index].behavior;

      numEntries++;
    }
    else
    {
      Serial.print(F("Removing color mapping config file index "));
      Serial.println(String(index));
    }
  }

  if(!m_colorMappingConfigFile.write(&json))
  {
    onFileWriteError();
  }
}

void HSDConfig::writeDeviceMappingConfigFile()
{
  DynamicJsonBuffer jsonBuffer(JSON_BUFFER_DEVICE_MAPPING_CONFIG_FILE);
  JsonObject& json = jsonBuffer.createObject();

  int numEntries = 0; // needed for skipping empty entries
  
  for(int index = 0; index < m_numDeviceMappingEntries; index++)
  {
    if(strlen(m_cfgDeviceMapping[index].name) != 0)
    {
      Serial.print(F("Preparing to write device mapping config file index "));
      Serial.println(String(index));
        
      JsonObject& deviceMappingEntry = json.createNestedObject(String(index));
  
      deviceMappingEntry[JSON_KEY_DEVICEMAPPING_NAME] = m_cfgDeviceMapping[index].name;
      deviceMappingEntry[JSON_KEY_DEVICEMAPPING_TYPE] = (int)m_cfgDeviceMapping[index].type;
      deviceMappingEntry[JSON_KEY_DEVICEMAPPING_LED]  = (int)m_cfgDeviceMapping[index].ledNumber;
  
      numEntries++;
    }
    else
    {
      Serial.print(F("Removing device mapping config file index "));
      Serial.println(String(index));
    }
  }
  
  if(!m_deviceMappingConfigFile.write(&json))
  {
    onFileWriteError();
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

void HSDConfig::onFileWriteError()
{
  Serial.println(F("Failed to write file, formatting file system."));
  SPIFFS.format();
  Serial.println(F("Done.")); 
}

bool HSDConfig::addDeviceMappingEntry(String name, deviceType type, int ledNumber)
{
  bool success = false;

  if(m_numDeviceMappingEntries < (MAX_DEVICE_MAP_ENTRIES - 1))
  {
    Serial.print(F("Adding device mapping entry at index ")); 
    Serial.println(String(m_numDeviceMappingEntries) + " with name " + name + ", type " + String(type) + ", LED number " + String(ledNumber));
  
    strncpy(m_cfgDeviceMapping[m_numDeviceMappingEntries].name, name.c_str(), MAX_DEVICE_MAPPING_NAME_LEN);
    m_cfgDeviceMapping[m_numDeviceMappingEntries].name[MAX_DEVICE_MAPPING_NAME_LEN] = '\0';
    
    m_cfgDeviceMapping[m_numDeviceMappingEntries].type = type;
    m_cfgDeviceMapping[m_numDeviceMappingEntries].ledNumber = ledNumber;
    m_numDeviceMappingEntries++;
    success = true;
  }
  else
  {
    Serial.print(F("Cannot add device mapping entry at index ")); 
    Serial.println(String(m_numDeviceMappingEntries));
  }

  return success;
}

bool HSDConfig::deleteDeviceMappingEntry(int entryNum)
{
  bool success = false;

  if( (entryNum >= 0) && (entryNum < m_numDeviceMappingEntries) )
  {
    memset(m_cfgDeviceMapping[entryNum].name, 0, MAX_DEVICE_MAPPING_NAME_LEN);
 
    success = true;
  }

  return success;  
}

bool HSDConfig::addColorMappingEntry(int entryNum, String msg, deviceType type, Color color, Behavior behavior)
{
  bool success = false;
  bool add = false;
  bool edit = false;

  if(entryNum >= m_numColorMappingEntries)
  {
    // new entry, ignore if entrynum (= index) is higher than next index, use this next index then -
    // but only if it is allowed to add further entries
    if(m_numColorMappingEntries < MAX_COLOR_MAP_ENTRIES)
    {
      entryNum = m_numColorMappingEntries;
      m_numColorMappingEntries++;
      add = true;

      Serial.print(F("Adding color mapping entry at index ")); 
      Serial.println(String(entryNum) + ", values: name " + msg + ", type " + String(type) + ", color " + String(color) + ", behavior " + String(behavior));
    }
  }
  else if(entryNum >= 0 && entryNum < m_numColorMappingEntries)
  {
    edit = true;

    Serial.print(F("Editing color mapping entry at index ")); 
    Serial.println(String(entryNum) + ", values: name " + msg + ", type " + String(type) + ", color " + String(color) + ", behavior " + String(behavior));
  }

  if(add || edit)
  {
    strncpy(m_cfgColorMapping[entryNum].msg, msg.c_str(), MAX_COLOR_MAPPING_MSG_LEN);
    m_cfgColorMapping[entryNum].msg[MAX_COLOR_MAPPING_MSG_LEN] = '\0';

    m_cfgColorMapping[entryNum].type = type;
    m_cfgColorMapping[entryNum].color = color;
    m_cfgColorMapping[entryNum].behavior = behavior;
    
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
  bool success = false;

  if( (entryNum >= 0) && (entryNum < m_numColorMappingEntries) )
  {
    memset(m_cfgColorMapping[entryNum].msg, 0, MAX_COLOR_MAPPING_MSG_LEN);
 
    success = true;
  }

  return success;
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

int HSDConfig::getNumberOfColorMappingEntries() const
{
  return m_numColorMappingEntries;
}

const HSDConfig::colorMapping* HSDConfig::getColorMapping(int index) const
{
  const colorMapping* mapping = NULL;

  if(index < m_numColorMappingEntries)
  {
    mapping = &m_cfgColorMapping[index];
  }

  return mapping;
}

int HSDConfig::getNumberOfDeviceMappingEntries() const
{
  return m_numDeviceMappingEntries;
}

const HSDConfig::deviceMapping* HSDConfig::getDeviceMapping(int index) const
{
  const deviceMapping* mapping = NULL;

  if(index < m_numDeviceMappingEntries)
  {
    mapping = &m_cfgDeviceMapping[index];
  }

  return mapping;
}

int HSDConfig::getLedNumber(String deviceName, deviceType deviceType)
{
  int number = -1;

  for(int i = 0; i < m_numDeviceMappingEntries; i++)
  {
    if(deviceName.equals(m_cfgDeviceMapping[i].name) && (deviceType == m_cfgDeviceMapping[i].type))
    {
      number = m_cfgDeviceMapping[i].ledNumber;
      break;
    }
  }

  return number;
}

int HSDConfig::getColorMapIndex(deviceType deviceType, String msg)
{
  int index = -1;

  for(int i = 0; i < m_numColorMappingEntries; i++)
  {
    if(msg.equals(m_cfgColorMapping[i].msg) && (deviceType == m_cfgColorMapping[i].type))
    {
      index = i;
      break;
    }
  }

  return index;
}

HSDConfig::Behavior HSDConfig::getLedBehavior(int colorMapIndex)
{
  return m_cfgColorMapping[colorMapIndex].behavior;
}

HSDConfig::Color HSDConfig::getLedColor(int colorMapIndex)
{
  return m_cfgColorMapping[colorMapIndex].color;
}

