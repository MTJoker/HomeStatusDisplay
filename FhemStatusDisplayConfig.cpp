#include "FhemStatusDisplayConfig.h"
#include <FS.h>
#include <ArduinoJson.h>

#define CONFIG_FILE_NAME_MAIN (F("/config.json"))
#define CONFIG_FILE_NAME_DEVICEMAPPING (F( "/devicemapping.json"))
#define CONFIG_FILE_NAME_COLORMAPPING (F("/colormapping.json"))

FhemStatusDisplayConfig::FhemStatusDisplayConfig()
{  
  // reset non-configuraable members
  setVersion("");
  setHost("");

  // reset configurable members
  resetMainConfigData();
  resetColorMappingConfigData();
}

void FhemStatusDisplayConfig::begin(const char* version, const char* defaultIdentifier)
{
  Serial.println(F(""));
  Serial.println(F("Initializing config."));

  setVersion(version);
  setHost(defaultIdentifier);

  // TODO: read from config file

  // 1st row
  addDeviceMappingEntry(F("basement"),   TYPE_DOOR,    0);
  addDeviceMappingEntry(F("main"),       TYPE_DOOR,    1);
  addDeviceMappingEntry(F("kitchen"),    TYPE_WINDOW,  2);
  addDeviceMappingEntry(F("eating"),     TYPE_WINDOW,  3);
  addDeviceMappingEntry(F("terrace"),    TYPE_DOOR,    4);
  addDeviceMappingEntry(F("bath_left"),  TYPE_WINDOW,  5);
  addDeviceMappingEntry(F("bath_right"), TYPE_WINDOW,  6);
  addDeviceMappingEntry(F("child"),      TYPE_WINDOW,  7);
  addDeviceMappingEntry(F("sleep"),      TYPE_WINDOW,  8);
  addDeviceMappingEntry(F("work"),       TYPE_WINDOW,  9);
  addDeviceMappingEntry(F("garage"),     TYPE_DOOR,   10);

  //2nd row
  addDeviceMappingEntry(F("kitchen_ceiling_right"), TYPE_LIGHT, 11);
  addDeviceMappingEntry(F("kitchen_ceiling_left"),  TYPE_LIGHT, 12);
  addDeviceMappingEntry(F("eating_ceiling"),        TYPE_LIGHT, 13);
  addDeviceMappingEntry(F("living_ceiling"),        TYPE_LIGHT, 14);
  addDeviceMappingEntry(F("living_stonewall"),      TYPE_LIGHT, 15);
  addDeviceMappingEntry(F("living_lowboard"),       TYPE_LIGHT, 16);
  addDeviceMappingEntry(F("living_shelf"),          TYPE_LIGHT, 17);
  addDeviceMappingEntry(F("bath_ceiling"),          TYPE_LIGHT, 18);
  addDeviceMappingEntry(F("child_ceiling"),         TYPE_LIGHT, 19);
  addDeviceMappingEntry(F("sleep_ceiling"),         TYPE_LIGHT, 20);
  addDeviceMappingEntry(F("work_ceiling"),          TYPE_LIGHT, 21);

  //3rd row
  addDeviceMappingEntry(F("washing_machine"),    TYPE_ALARM, 22);
  addDeviceMappingEntry(F("waterdetector_1"),    TYPE_ALARM, 23);
  addDeviceMappingEntry(F("waterdetector_2"),    TYPE_ALARM, 24);
  addDeviceMappingEntry(F("oven"),               TYPE_ALARM, 25);
  addDeviceMappingEntry(F("waste_residual_bio"), TYPE_ALARM, 26);
  addDeviceMappingEntry(F("waste_paper_yellow"), TYPE_ALARM, 27);
  addDeviceMappingEntry(F("device_error"),       TYPE_ALARM, 28);
  addDeviceMappingEntry(F("battery_error"),      TYPE_ALARM, 29);
  addDeviceMappingEntry(F("unused_3"),           TYPE_ALARM, 30);
  addDeviceMappingEntry(F("unused_2"),           TYPE_ALARM, 31);
  addDeviceMappingEntry(F("unused_1"),           TYPE_ALARM, 32);
/*
  addColorMappingEntry("open",     TYPE_WINDOW, Led::BLUE,   Led::ON); 
  addColorMappingEntry("closed",   TYPE_WINDOW, Led::NONE,   Led::OFF); 
  addColorMappingEntry("tilted",   TYPE_WINDOW, Led::YELLOW, Led::ON); 
  addColorMappingEntry("open",     TYPE_DOOR,   Led::BLUE,   Led::ON); 
  addColorMappingEntry("closed",   TYPE_DOOR,   Led::NONE,   Led::OFF); 
  addColorMappingEntry("tilted",   TYPE_DOOR,   Led::YELLOW, Led::ON); 
  addColorMappingEntry("on",       TYPE_LIGHT,  Led::GREEN,  Led::ON); 
  addColorMappingEntry("off",      TYPE_LIGHT,  Led::NONE,   Led::OFF); 
  addColorMappingEntry("true",     TYPE_ALARM,  Led::YELLOW, Led::FLASHING); // used by oven, defectDevices, battery
  addColorMappingEntry("false",    TYPE_ALARM,  Led::NONE,   Led::OFF);      // used by oven, defectDevices, battery
  addColorMappingEntry("closed",   TYPE_ALARM,  Led::RED,    Led::BLINKING); // used by water detectors
  addColorMappingEntry("open",     TYPE_ALARM,  Led::NONE,   Led::OFF);      // used by water detectors
  addColorMappingEntry("on",       TYPE_ALARM,  Led::GREEN,  Led::ON);       // used by washing machine
  addColorMappingEntry("off",      TYPE_ALARM,  Led::NONE,   Led::OFF);      // used by washing machine
  addColorMappingEntry("standby",  TYPE_ALARM,  Led::YELLOW, Led::BLINKING); // used by washing machine
  addColorMappingEntry("today",    TYPE_ALARM,  Led::RED,    Led::ON);       // used by waste
  addColorMappingEntry("tomorrow", TYPE_ALARM,  Led::YELLOW, Led::ON);       // used by waste
  addColorMappingEntry("none",     TYPE_ALARM,  Led::NONE,   Led::OFF);      // used by waste
*/
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
  }
  else
  {
    Serial.println(F("Failed to mount file system"));
  }
}

void FhemStatusDisplayConfig::resetMainConfigData()
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

void FhemStatusDisplayConfig::resetColorMappingConfigData()
{
  Serial.println(F("Deleting color mapping config data."));
  
  memset(m_cfgColorMapping, 0, sizeof(m_cfgColorMapping));
  memset(m_cfgDeviceMapping, 0, sizeof(m_cfgDeviceMapping));
  m_numColorMappingEntries = 0;
}

void FhemStatusDisplayConfig::createDefaultMainConfigFile()
{
  Serial.println(F("Creating default main config file."));
  resetMainConfigData();
  writeMainConfigFile();
}

void FhemStatusDisplayConfig::createDefaultColorMappingConfigFile()
{
  Serial.println(F("Creating default color mapping config file."));
  resetColorMappingConfigData();
  writeColorMappingConfigFile();
}

bool FhemStatusDisplayConfig::readMainConfigFile()
{
  bool success = false;
  
  Serial.println(F("Reading main config file."));  
        
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_MAIN, "r");

  if(configFile)
  {
    size_t size = configFile.size();
    Serial.print(F("Opened main config file, size is ")); Serial.println(String(size) + " bytes");  

    // allocate buffer for the file contents
    std::unique_ptr<char[]> buf(new char[size]);

    configFile.readBytes(buf.get(), size);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(buf.get());

    if (json.success()) 
    {
      Serial.println(F("Main config data successfully parsed."));

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
    Serial.println(F("Failed to open config file."));
  }

  return success;
}

bool FhemStatusDisplayConfig::readColorMappingConfigFile()
{
  bool success = false;
  
  Serial.println(F("Reading color mapping config file."));  
        
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_COLORMAPPING, "r");

  if(configFile)
  {
    size_t size = configFile.size();
    Serial.print(F("Opened color mapping config file, size is ")); Serial.println(String(size) + " bytes");  

    // allocate buffer for the file contents
    std::unique_ptr<char[]> buf(new char[size]);

    configFile.readBytes(buf.get(), size);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(buf.get());

    if (json.success()) 
    {
      Serial.println(F("Color mapping config data successfully parsed."));

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
                               (Led::Color)(entry[JSON_KEY_COLORMAPPING_COLOR].as<int>()), 
                               (Led::Behavior)(entry[JSON_KEY_COLORMAPPING_BEHAVIOR].as<int>())); 
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
      Serial.println(F("Could not read config data"));
    }
  }
  else
  {
    Serial.println(F("Failed to open config file."));
  }

  return success;
}

void FhemStatusDisplayConfig::writeMainConfigFile()
{
  Serial.println(F("Writing main config file."));  

  DynamicJsonBuffer jsonBuffer;
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

void FhemStatusDisplayConfig::writeColorMappingConfigFile()
{
  Serial.println(F("Writing color mapping config file."));  

  DynamicJsonBuffer jsonBuffer;
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

void FhemStatusDisplayConfig::saveMain()
{
  writeMainConfigFile();
}

void FhemStatusDisplayConfig::saveColorMapping()
{
  writeColorMappingConfigFile();
}

bool FhemStatusDisplayConfig::addDeviceMappingEntry(String name, deviceType type, int ledNumber)
{
  bool success = false;

  if(m_numDeviceMappingEntries < (MAX_DEVICE_MAP_ENTRIES - 1))
  {
    m_cfgDeviceMapping[m_numDeviceMappingEntries].name = name;
    m_cfgDeviceMapping[m_numDeviceMappingEntries].type = type;
    m_cfgDeviceMapping[m_numDeviceMappingEntries].ledNumber = ledNumber;
    m_numDeviceMappingEntries++;
    success = true;
  }

  return success;
}

bool FhemStatusDisplayConfig::addColorMappingEntry(String msg, deviceType type, Led::Color color, Led::Behavior behavior)
{
  bool success = false;

  Serial.print(F("Adding color mapping entry at index ")); 
  Serial.println(String(m_numColorMappingEntries) + " with name " + msg + ", type " + String(type) + ", color " + String(color) + ", behavior " + String(behavior));

  if(m_numColorMappingEntries < (MAX_COLOR_MAP_ENTRIES - 1))
  {
    m_cfgColorMapping[m_numColorMappingEntries].msg = msg;
    m_cfgColorMapping[m_numColorMappingEntries].type = type;
    m_cfgColorMapping[m_numColorMappingEntries].color = color;
    m_cfgColorMapping[m_numColorMappingEntries].behavior = behavior;
    m_numColorMappingEntries++;
    success = true;
  }
  
  return success;  
}

const char* FhemStatusDisplayConfig::getHost() const
{
  return m_cfgHost;
}

bool FhemStatusDisplayConfig::setHost(const char* host)
{
  strncpy(m_cfgHost, host, MAX_HOST_LEN);
  m_cfgHost[MAX_HOST_LEN] = '\0';
  return true;
}

const char* FhemStatusDisplayConfig::getVersion() const
{
  return m_cfgVersion;
}

bool FhemStatusDisplayConfig::setVersion(const char* version)
{
  strncpy(m_cfgVersion, version, MAX_VERSION_LEN);
  m_cfgVersion[MAX_VERSION_LEN] = '\0';
  return true;
}

const char* FhemStatusDisplayConfig::getWifiSSID() const
{
  return m_cfgWifiSSID;
}

bool FhemStatusDisplayConfig::setWifiSSID(const char* ssid)
{
  strncpy(m_cfgWifiSSID, ssid, MAX_WIFI_SSID_LEN);
  m_cfgWifiSSID[MAX_WIFI_SSID_LEN] = '\0';
  return true;
}

const char* FhemStatusDisplayConfig::getWifiPSK() const
{
  return m_cfgWifiPSK;
}

bool FhemStatusDisplayConfig::setWifiPSK(const char* psk)
{
  strncpy(m_cfgWifiPSK, psk, MAX_WIFI_PSK_LEN);
  m_cfgWifiPSK[MAX_WIFI_PSK_LEN] = '\0';
  return true;
}

const char* FhemStatusDisplayConfig::getMqttServer() const
{
  return m_cfgMqttServer;
}

bool FhemStatusDisplayConfig::setMqttServer(const char* ip)
{
  strncpy(m_cfgMqttServer, ip, MAX_MQTT_SERVER_LEN);
  m_cfgMqttServer[MAX_MQTT_SERVER_LEN] = '\0';
  return true;
}

const char* FhemStatusDisplayConfig::getMqttStatusTopic() const
{
  return m_cfgMqttStatusTopic;
}

bool FhemStatusDisplayConfig::setMqttStatusTopic(const char* topic)
{
  strncpy(m_cfgMqttStatusTopic, topic, MAX_MQTT_STATUS_TOPIC_LEN);
  m_cfgMqttStatusTopic[MAX_MQTT_STATUS_TOPIC_LEN] = '\0';
  return true;
}

const char* FhemStatusDisplayConfig::getMqttTestTopic() const
{
  return m_cfgMqttTestTopic;
}

bool FhemStatusDisplayConfig::setMqttTestTopic(const char* topic)
{
  strncpy(m_cfgMqttTestTopic, topic, MAX_MQTT_TEST_TOPIC_LEN);
  m_cfgMqttTestTopic[MAX_MQTT_TEST_TOPIC_LEN] = '\0';
  return true;
}

int FhemStatusDisplayConfig::getNumberOfLeds() const
{
  return m_cfgNumberOfLeds;
}

const char* FhemStatusDisplayConfig::getMqttWillTopic() const
{
  return m_cfgMqttWillTopic;
}

bool FhemStatusDisplayConfig::setMqttWillTopic(const char* topic)
{
  strncpy(m_cfgMqttWillTopic, topic, MAX_MQTT_WILL_TOPIC_LEN);
  m_cfgMqttWillTopic[MAX_MQTT_WILL_TOPIC_LEN] = '\0';
  return true;
}

bool FhemStatusDisplayConfig::setNumberOfLeds(uint32_t numberOfLeds)
{
  m_cfgNumberOfLeds = numberOfLeds;
  return true;
}

int FhemStatusDisplayConfig::getLedDataPin() const
{
  return m_cfgLedDataPin;
}

bool FhemStatusDisplayConfig::setLedDataPin(int dataPin)
{
  m_cfgLedDataPin = dataPin;
  return true;
}

int FhemStatusDisplayConfig::getNumberOfColorMappingEntries() const
{
  return m_numColorMappingEntries;
}

const colorMapping* FhemStatusDisplayConfig::getColorMapping(int index) const
{
  const colorMapping* mapping = NULL;

  if(index < m_numColorMappingEntries)
  {
    mapping = &m_cfgColorMapping[index];
  }

  return mapping;
}

int FhemStatusDisplayConfig::getLedNumber(String deviceName, deviceType deviceType)
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

int FhemStatusDisplayConfig::getColorMapIndex(deviceType deviceType, String msg)
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

Led::Behavior FhemStatusDisplayConfig::getLedBehavior(int colorMapIndex)
{
  return m_cfgColorMapping[colorMapIndex].behavior;
}

Led::Color FhemStatusDisplayConfig::getLedColor(int colorMapIndex)
{
  return m_cfgColorMapping[colorMapIndex].color;
}

