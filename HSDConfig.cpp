#include "HSDConfig.h"
#include <FS.h>
#include <ArduinoJson.h>

#define CONFIG_FILE_NAME_MAIN (F("/config.json"))
#define CONFIG_FILE_NAME_DEVICEMAPPING (F( "/devicemapping.json"))
#define CONFIG_FILE_NAME_COLORMAPPING (F("/colormapping.json"))

HSDConfig::HSDConfig()
{  
  // reset non-configuraable members
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

    if(!SPIFFS.exists(CONFIG_FILE_NAME_MAIN) || !readMainConfigFile())
    {
      createDefaultMainConfigFile();
    }

    if(!SPIFFS.exists(CONFIG_FILE_NAME_COLORMAPPING) || !readColorMappingConfigFile())
    {
      createDefaultColorMappingConfigFile();
    }

    if(!SPIFFS.exists(CONFIG_FILE_NAME_DEVICEMAPPING) || !readDeviceMappingConfigFile())
    {
      createDefaultDeviceMappingConfigFile();
    }
  }
  else
  {
    Serial.println(F("Failed to mount file system"));
  }
}

void HSDConfig::resetMainConfigData()
{
  setWifiSSID("");
  setWifiPSK("");

  setMqttServer("");
  setMqttStatusTopic("");
  setMqttTestTopic("");  
  setMqttWillTopic(""); 

  setNumberOfLeds(0);
  setLedDataPin(0);
}

void HSDConfig::resetColorMappingConfigData()
{
  Serial.println(F("Deleting color mapping config data."));
  
  memset(m_cfgColorMapping, 0, sizeof(m_cfgColorMapping));
  memset(m_cfgDeviceMapping, 0, sizeof(m_cfgDeviceMapping));
  m_numColorMappingEntries = 0;
}

void HSDConfig::resetDeviceMappingConfigData()
{
  Serial.println(F("Deleting device mapping config data."));
  
  memset(m_cfgDeviceMapping, 0, sizeof(m_cfgDeviceMapping));
  memset(m_cfgDeviceMapping, 0, sizeof(m_cfgDeviceMapping));
  m_numDeviceMappingEntries = 0;  
}

void HSDConfig::createDefaultMainConfigFile()
{
  Serial.println(F("Creating default main config file."));
  resetMainConfigData();
  writeMainConfigFile();
}

void HSDConfig::createDefaultColorMappingConfigFile()
{
  Serial.println(F("Creating default color mapping config file."));
  resetColorMappingConfigData();
  writeColorMappingConfigFile();
}

void HSDConfig::createDefaultDeviceMappingConfigFile()
{
  Serial.println(F("Creating default device mapping config file."));
  resetDeviceMappingConfigData();
  writeDeviceMappingConfigFile();
}

bool HSDConfig::readMainConfigFile()
{
  bool success = false;
  
  Serial.println(F("Reading main config file."));  
        
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_MAIN, "r");

  if(configFile)
  {
    size_t size = configFile.size();
    Serial.print(F("Opened main config file, size is ")); Serial.println(String(size) + " bytes");  

    if(size <= MAX_SIZE_COLOR_MAPPING_CONFIG)
    { 
      char buffer[MAX_SIZE_MAIN_CONFIG];
  
      configFile.readBytes(buffer, size);
  
      DynamicJsonBuffer jsonBuffer(MAX_SIZE_MAIN_CONFIG);
      JsonObject& json = jsonBuffer.parseObject(buffer);
  
      if (json.success()) 
      {
        Serial.println(F("Main config data successfully parsed."));
        Serial.print(F("JSON length is ")); Serial.println(json.measureLength());  
        
        json.prettyPrintTo(Serial);
        Serial.println("");
  
        if(json.containsKey(JSON_KEY_HOST) && json.containsKey(JSON_KEY_WIFI_SSID) && json.containsKey(JSON_KEY_WIFI_PSK) && 
           json.containsKey(JSON_KEY_MQTT_SERVER) && json.containsKey(JSON_KEY_MQTT_STATUS_TOPIC) && json.containsKey(JSON_KEY_MQTT_TEST_TOPIC) && json.containsKey(JSON_KEY_MQTT_WILL_TOPIC) &&
           json.containsKey(JSON_KEY_LED_COUNT) && json.containsKey(JSON_KEY_LED_PIN))
        {
          Serial.println(F("All config file keys available."));
  
          setHost(json[JSON_KEY_HOST]);
          setWifiSSID(json[JSON_KEY_WIFI_SSID]);
          setWifiPSK(json[JSON_KEY_WIFI_PSK]);
          setMqttServer(json[JSON_KEY_MQTT_SERVER]);
          setMqttStatusTopic(json[JSON_KEY_MQTT_STATUS_TOPIC]);
          setMqttTestTopic(json[JSON_KEY_MQTT_TEST_TOPIC]);
          setMqttWillTopic(json[JSON_KEY_MQTT_WILL_TOPIC]);
          setNumberOfLeds(json[JSON_KEY_LED_COUNT]);
          setLedDataPin(json[JSON_KEY_LED_PIN]);
  
          success = true;
        }
        else
        {
          Serial.println(F("Missing config file keys!"));
        }
      } 
      else 
      {
        Serial.println(F("Could not read config data"));
      }
    }
    else
    {
      Serial.println(F("Config file is too big."));
    }
  }
  else
  {
    Serial.println(F("Failed to open config file."));
  }

  return success;
}

bool HSDConfig::readColorMappingConfigFile()
{
  bool success = false;
  
  Serial.println(F("Reading color mapping config file."));  
        
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_COLORMAPPING, "r");

  if(configFile)
  {
    size_t size = configFile.size();
    Serial.print(F("Opened color mapping config file, size is ")); Serial.println(String(size) + " bytes");  

    if(size <= MAX_SIZE_COLOR_MAPPING_CONFIG)
    { 
      char buffer[MAX_SIZE_COLOR_MAPPING_CONFIG];
  
      configFile.readBytes(buffer, size);
  
      DynamicJsonBuffer jsonBuffer(MAX_SIZE_COLOR_MAPPING_CONFIG);
      JsonObject& json = jsonBuffer.parseObject(buffer);
  
      if (json.success()) 
      {
        Serial.println(F("Color mapping config data successfully parsed."));
        Serial.print(F("JSON length is ")); Serial.println(json.measureLength());  
  
        json.prettyPrintTo(Serial);
        Serial.println("");
  
        success = true;
        
        for(JsonObject::iterator it = json.begin(); it != json.end(); ++it)
        {
          JsonObject& entry = json[it->key]; 
  
          if(entry.containsKey(JSON_KEY_COLORMAPPING_MSG) && entry.containsKey(JSON_KEY_COLORMAPPING_TYPE) &&
             entry.containsKey(JSON_KEY_COLORMAPPING_COLOR) && entry.containsKey(JSON_KEY_COLORMAPPING_BEHAVIOR) )
          {
            addColorMappingEntry(entry[JSON_KEY_COLORMAPPING_MSG].asString(), 
                                 (deviceType)(entry[JSON_KEY_COLORMAPPING_TYPE].as<int>()), 
                                 (HSDLed::Color)(entry[JSON_KEY_COLORMAPPING_COLOR].as<int>()), 
                                 (HSDLed::Behavior)(entry[JSON_KEY_COLORMAPPING_BEHAVIOR].as<int>())); 
          }
          else
          {
            Serial.println(F("Missing config file keys!"));
            success = false;
            break;
          }
        }
      }
      else
      {
        Serial.println(F("Could not read config data."));
      }
    } 
    else 
    {
      Serial.println(F("Config file is too big."));
    }
  }
  else
  {
    Serial.println(F("Failed to open config file."));
  }

  return success;
}

bool HSDConfig::readDeviceMappingConfigFile()
{
  bool success = false;
  
  Serial.println(F("Reading device mapping config file."));  
      
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_DEVICEMAPPING, "r");

  if(configFile)
  {
    size_t size = configFile.size();
    Serial.print(F("Opened device mapping config file, size is ")); Serial.println(String(size) + " bytes");  

    if(size <= MAX_SIZE_DEVICE_MAPPING_CONFIG)
    { 
      char buffer[MAX_SIZE_DEVICE_MAPPING_CONFIG];
  
      configFile.readBytes(buffer, size);
  
      DynamicJsonBuffer jsonBuffer(MAX_SIZE_DEVICE_MAPPING_CONFIG);
      JsonObject& json = jsonBuffer.parseObject(buffer);
  
      if (json.success()) 
      {
        Serial.println(F("Device mapping config data successfully parsed."));
        Serial.print(F("JSON length is ")); Serial.println(json.measureLength());  
  
        json.prettyPrintTo(Serial);
        Serial.println("");
  
        success = true;
        
        for(JsonObject::iterator it = json.begin(); it != json.end(); ++it)
        {
          JsonObject& entry = json[it->key]; 
  
          if(entry.containsKey(JSON_KEY_DEVICEMAPPING_NAME) && entry.containsKey(JSON_KEY_DEVICEMAPPING_TYPE) &&
             entry.containsKey(JSON_KEY_DEVICEMAPPING_LED) )
          {
            addDeviceMappingEntry(entry[JSON_KEY_DEVICEMAPPING_NAME].asString(), 
                                 (deviceType)(entry[JSON_KEY_DEVICEMAPPING_TYPE].as<int>()), 
                                 entry[JSON_KEY_DEVICEMAPPING_LED].as<int>()); 
          }
          else
          {
            Serial.println(F("Missing config file keys!"));
            success = false;
            break;
          }
        }
      }
      else
      {
        Serial.println(F("Could not read config data."));
      }
    } 
    else 
    {
      Serial.println(F("Config file is too big."));
    }
  }
  else
  {
    Serial.println(F("Failed to open config file."));
  }

  return success;
}

void HSDConfig::writeMainConfigFile()
{
  Serial.println(F("Writing main config file."));  

  DynamicJsonBuffer jsonBuffer(MAX_SIZE_MAIN_CONFIG);
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
  
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_MAIN, "w");
  
  if (!configFile) 
  {
    Serial.println(F("Failed to write main config file, formatting file system."));
    SPIFFS.format();
    Serial.println(F("Done."));
  }

  json.prettyPrintTo(Serial);
  Serial.println(F(""));
  
  json.printTo(configFile);
  configFile.close();
}

void HSDConfig::writeColorMappingConfigFile()
{
  Serial.println(F("Writing color mapping config file."));  

  DynamicJsonBuffer jsonBuffer(MAX_SIZE_COLOR_MAPPING_CONFIG);
  JsonObject& json = jsonBuffer.createObject();

  for(int index = 0; index < m_numColorMappingEntries; index++)
  {
    JsonObject& colorMappingEntry = json.createNestedObject(String(index));

    colorMappingEntry[JSON_KEY_COLORMAPPING_MSG] = m_cfgColorMapping[index].msg ;
    colorMappingEntry[JSON_KEY_COLORMAPPING_TYPE] = (int)m_cfgColorMapping[index].type;
    colorMappingEntry[JSON_KEY_COLORMAPPING_COLOR] = (int)m_cfgColorMapping[index].color;
    colorMappingEntry[JSON_KEY_COLORMAPPING_BEHAVIOR] = (int)m_cfgColorMapping[index].behavior;
  }
  
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_COLORMAPPING, "w");
  
  if (!configFile) 
  {
    Serial.println(F("Failed to write color mapping config file, formatting file system."));
    SPIFFS.format();
    Serial.println(F("Done."));
  }

  json.prettyPrintTo(Serial);
  Serial.println("");
  
  json.printTo(configFile);
  configFile.close();  
}

void HSDConfig::writeDeviceMappingConfigFile()
{
  Serial.println(F("Writing device mapping config file."));  

  DynamicJsonBuffer jsonBuffer(MAX_SIZE_DEVICE_MAPPING_CONFIG);
  JsonObject& json = jsonBuffer.createObject();

  for(int index = 0; index < m_numDeviceMappingEntries; index++)
  {
    JsonObject& colorMappingEntry = json.createNestedObject(String(index));

    colorMappingEntry[JSON_KEY_DEVICEMAPPING_NAME] = m_cfgDeviceMapping[index].name;
    colorMappingEntry[JSON_KEY_DEVICEMAPPING_TYPE] = (int)m_cfgDeviceMapping[index].type;
    colorMappingEntry[JSON_KEY_DEVICEMAPPING_LED]  = (int)m_cfgDeviceMapping[index].ledNumber;
  }
  
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_DEVICEMAPPING, "w");
  
  if (!configFile) 
  {
    Serial.println(F("Failed to write device mapping config file, formatting file system."));
    SPIFFS.format();
    Serial.println(F("Done."));
  }

  json.prettyPrintTo(Serial);
  Serial.println("");
  
  json.printTo(configFile);
  configFile.close();  
}

void HSDConfig::saveMain()
{
  writeMainConfigFile();
}

void HSDConfig::saveColorMapping()
{
  writeColorMappingConfigFile();
}

void HSDConfig::saveDeviceMapping()
{
  writeDeviceMappingConfigFile();
}

bool HSDConfig::addDeviceMappingEntry(String name, deviceType type, int ledNumber)
{
  bool success = false;

  Serial.print(F("Adding device mapping entry at index ")); 
  Serial.println(String(m_numDeviceMappingEntries) + " with name " + name + ", type " + String(type) + ", LED number " + String(ledNumber));

  if(m_numDeviceMappingEntries < (MAX_DEVICE_MAP_ENTRIES - 1))
  {
    strncpy(m_cfgDeviceMapping[m_numDeviceMappingEntries].name, name.c_str(), MAX_DEVICE_MAPPING_NAME_LEN);
    m_cfgDeviceMapping[m_numDeviceMappingEntries].name[MAX_DEVICE_MAPPING_NAME_LEN] = '\0';
    
    m_cfgDeviceMapping[m_numDeviceMappingEntries].type = type;
    m_cfgDeviceMapping[m_numDeviceMappingEntries].ledNumber = ledNumber;
    m_numDeviceMappingEntries++;
    success = true;
  }

  return success;
}

bool HSDConfig::addColorMappingEntry(String msg, deviceType type, HSDLed::Color color, HSDLed::Behavior behavior)
{
  bool success = false;

  Serial.print(F("Adding color mapping entry at index ")); 
  Serial.println(String(m_numColorMappingEntries) + " with name " + msg + ", type " + String(type) + ", color " + String(color) + ", behavior " + String(behavior));

  if(m_numColorMappingEntries < (MAX_COLOR_MAP_ENTRIES - 1))
  {
    strncpy(m_cfgColorMapping[m_numColorMappingEntries].msg, msg.c_str(), MAX_COLOR_MAPPING_MSG_LEN);
    m_cfgColorMapping[m_numColorMappingEntries].msg[MAX_COLOR_MAPPING_MSG_LEN] = '\0';

    m_cfgColorMapping[m_numColorMappingEntries].type = type;
    m_cfgColorMapping[m_numColorMappingEntries].color = color;
    m_cfgColorMapping[m_numColorMappingEntries].behavior = behavior;
    m_numColorMappingEntries++;
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

HSDLed::Behavior HSDConfig::getLedBehavior(int colorMapIndex)
{
  return m_cfgColorMapping[colorMapIndex].behavior;
}

HSDLed::Color HSDConfig::getLedColor(int colorMapIndex)
{
  return m_cfgColorMapping[colorMapIndex].color;
}

