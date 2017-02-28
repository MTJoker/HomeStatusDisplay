#include "FhemStatusDisplayConfig.h"
#include <FS.h>
#include <ArduinoJson.h>

static const char* CONFIG_FILE_NAME_MAIN = "/config.json";
static const char* CONFIG_FILE_NAME_DEVICEMAPPING = "/devicemapping.json";
static const char* CONFIG_FILE_NAME_COLORMAPPING = "/colormapping.json";

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
  Serial.println("");
  Serial.println("Initializing config.");

  setVersion(version);
  setHost(defaultIdentifier);

  // TODO: read from config file

  // 1st row
  addDeviceMappingEntry("basement",   TYPE_DOOR,    0);
  addDeviceMappingEntry("main",       TYPE_DOOR,    1);
  addDeviceMappingEntry("kitchen",    TYPE_WINDOW,  2);
  addDeviceMappingEntry("eating",     TYPE_WINDOW,  3);
  addDeviceMappingEntry("terrace",    TYPE_DOOR,    4);
  addDeviceMappingEntry("bath_left",  TYPE_WINDOW,  5);
  addDeviceMappingEntry("bath_right", TYPE_WINDOW,  6);
  addDeviceMappingEntry("child",      TYPE_WINDOW,  7);
  addDeviceMappingEntry("sleep",      TYPE_WINDOW,  8);
  addDeviceMappingEntry("work",       TYPE_WINDOW,  9);
  addDeviceMappingEntry("garage",     TYPE_DOOR,   10);

  //2nd row
  addDeviceMappingEntry("kitchen_ceiling_right", TYPE_LIGHT, 11);
  addDeviceMappingEntry("kitchen_ceiling_left",  TYPE_LIGHT, 12);
  addDeviceMappingEntry("eating_ceiling",        TYPE_LIGHT, 13);
  addDeviceMappingEntry("living_ceiling",        TYPE_LIGHT, 14);
  addDeviceMappingEntry("living_stonewall",      TYPE_LIGHT, 15);
  addDeviceMappingEntry("living_lowboard",       TYPE_LIGHT, 16);
  addDeviceMappingEntry("living_shelf",          TYPE_LIGHT, 17);
  addDeviceMappingEntry("bath_ceiling",          TYPE_LIGHT, 18);
  addDeviceMappingEntry("child_ceiling",         TYPE_LIGHT, 19);
  addDeviceMappingEntry("sleep_ceiling",         TYPE_LIGHT, 20);
  addDeviceMappingEntry("work_ceiling",          TYPE_LIGHT, 21);

  //3rd row
  addDeviceMappingEntry("washing_machine",    TYPE_ALARM, 22);
  addDeviceMappingEntry("waterdetector_1",    TYPE_ALARM, 23);
  addDeviceMappingEntry("waterdetector_2",    TYPE_ALARM, 24);
  addDeviceMappingEntry("oven",               TYPE_ALARM, 25);
  addDeviceMappingEntry("waste_residual_bio", TYPE_ALARM, 26);
  addDeviceMappingEntry("waste_paper_yellow", TYPE_ALARM, 27);
  addDeviceMappingEntry("device_error",       TYPE_ALARM, 28);
  addDeviceMappingEntry("battery_error",      TYPE_ALARM, 29);
  addDeviceMappingEntry("unused_3",           TYPE_ALARM, 30);
  addDeviceMappingEntry("unused_2",           TYPE_ALARM, 31);
  addDeviceMappingEntry("unused_1",           TYPE_ALARM, 32);
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
    Serial.println("Mounted file system.");

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
    Serial.println("Failed to mount file system");
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
  memset(m_cfgColorMapping, 0, sizeof(m_cfgColorMapping));
  memset(m_cfgDeviceMapping, 0, sizeof(m_cfgDeviceMapping));
  m_numDeviceMappingEntries = 0;
  m_numColorMappingEntries = 0;
}

void FhemStatusDisplayConfig::createDefaultMainConfigFile()
{
  Serial.println("Creating default main config file.");
  resetMainConfigData();
  writeMainConfigFile();
}

void FhemStatusDisplayConfig::createDefaultColorMappingConfigFile()
{
  Serial.println("Creating default color mapping config file.");
  resetColorMappingConfigData();
  writeColorMappingConfigFile();
}

bool FhemStatusDisplayConfig::readMainConfigFile()
{
  bool success = false;
  
  Serial.println("Reading main config file.");  
        
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_MAIN, "r");

  if(configFile)
  {
    size_t size = configFile.size();
    Serial.printf("Opened main config file, size is %u bytes.\n", size);  

    // allocate buffer for the file contents
    std::unique_ptr<char[]> buf(new char[size]);

    configFile.readBytes(buf.get(), size);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(buf.get());

    if (json.success()) 
    {
      Serial.println("Main config data successfully parsed.");

      json.prettyPrintTo(Serial);
      Serial.println("");

      if(json.containsKey("host") && json.containsKey("wifiSSID") && json.containsKey("wifiPSK") && 
         json.containsKey("mqttServer") && json.containsKey("mqttStatusTopic") && json.containsKey("mqttTestTopic") && json.containsKey("mqttWillTopic") &&
         json.containsKey("ledCount") && json.containsKey("ledPin"))
      {
        Serial.println("All config file keys available.");

        setHost(json["host"]);
        setWifiSSID(json["wifiSSID"]);
        setWifiPSK(json["wifiPSK"]);
        setMqttServer(json["mqttServer"]);
        setMqttStatusTopic(json["mqttStatusTopic"]);
        setMqttTestTopic(json["mqttTestTopic"]);
        setMqttWillTopic(json["mqttWillTopic"]);
        setNumberOfLeds(json["ledCount"]);
        setLedDataPin(json["ledPin"]);

        success = true;
      }
      else
      {
        Serial.println("Missing config file keys!");
      }
    } 
    else 
    {
      Serial.println("Could not read config data");
    }
  }
  else
  {
    Serial.println("Failed to open config file.");
  }

  return success;
}

bool FhemStatusDisplayConfig::readColorMappingConfigFile()
{
  bool success = false;
  
  Serial.println("Reading color mapping config file.");  
        
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_COLORMAPPING, "r");

  if(configFile)
  {
    size_t size = configFile.size();
    Serial.printf("Opened color mapping config file, size is %u bytes.\n", size);  

    // allocate buffer for the file contents
    std::unique_ptr<char[]> buf(new char[size]);

    configFile.readBytes(buf.get(), size);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(buf.get());

    if (json.success()) 
    {
      Serial.println("Color mapping config data successfully parsed.");

      json.prettyPrintTo(Serial);
      Serial.println("");

      success = true;
            
      for(JsonObject::iterator it = json.begin(); it != json.end(); ++it)
      {
        JsonObject& entry = json[it->key]; 

        if(entry.containsKey("msg") && entry.containsKey("type") &&
           entry.containsKey("color") && entry.containsKey("behavior") )
        {
          addColorMappingEntry(entry["msg"].asString(), 
                               (deviceType)(entry["type"].as<int>()), 
                               (Led::Color)(entry["color"].as<int>()), 
                               (Led::Behavior)(entry["behavior"].as<int>())); 
        }
        else
        {
          Serial.println("Missing config file keys!");
          success = false;
          break;
        }
      }
    } 
    else 
    {
      Serial.println("Could not read config data");
    }
  }
  else
  {
    Serial.println("Failed to open config file.");
  }

  return success;
}

void FhemStatusDisplayConfig::writeMainConfigFile()
{
  Serial.println("Writing main config file.");  

  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  json["host"] = m_cfgHost;
  json["wifiSSID"] = m_cfgWifiSSID;
  json["wifiPSK"] = m_cfgWifiPSK;
  json["mqttServer"] = m_cfgMqttServer;
  json["mqttStatusTopic"] = m_cfgMqttStatusTopic;
  json["mqttTestTopic"] = m_cfgMqttTestTopic;
  json["mqttWillTopic"] = m_cfgMqttWillTopic;
  json["ledCount"] = m_cfgNumberOfLeds;
  json["ledPin"] = m_cfgLedDataPin;
  
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_MAIN, "w");
  
  if (!configFile) 
  {
    Serial.println("Failed to write main config file, formatting file system.");
    SPIFFS.format();
    Serial.println("Done.");
  }

  json.prettyPrintTo(Serial);
  Serial.println("");
  
  json.printTo(configFile);
  configFile.close();
}

void FhemStatusDisplayConfig::writeColorMappingConfigFile()
{
  Serial.println("Writing color mapping config file.");  

  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  for(uint32_t index = 0; index < m_numColorMappingEntries; index++)
  {
    JsonObject& colorMappingEntry = json.createNestedObject(String(index));
    colorMappingEntry["msg"] = m_cfgColorMapping[index].msg ;
    colorMappingEntry["type"] = (int)m_cfgColorMapping[index].type;
    colorMappingEntry["color"] = (int)m_cfgColorMapping[index].color;
    colorMappingEntry["behavior"] = (int)m_cfgColorMapping[index].behavior;
  }
  
  File configFile = SPIFFS.open(CONFIG_FILE_NAME_COLORMAPPING, "w");
  
  if (!configFile) 
  {
    Serial.println("Failed to write color mapping config file, formatting file system.");
    SPIFFS.format();
    Serial.println("Done.");
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

uint32_t FhemStatusDisplayConfig::getNumberOfLeds() const
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

uint32_t FhemStatusDisplayConfig::getLedDataPin() const
{
  return m_cfgLedDataPin;
}

bool FhemStatusDisplayConfig::setLedDataPin(uint32 dataPin)
{
  m_cfgLedDataPin = dataPin;
  return true;
}

uint32_t FhemStatusDisplayConfig::getNumberOfColorMappingEntries() const
{
  return m_numColorMappingEntries;
}

const colorMapping* FhemStatusDisplayConfig::getColorMapping(uint32_t index) const
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

  for(uint32_t i = 0; i < m_numDeviceMappingEntries; i++)
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

  for(uint32_t i = 0; i < m_numColorMappingEntries; i++)
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

