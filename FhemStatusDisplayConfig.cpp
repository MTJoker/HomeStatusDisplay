#include "FhemStatusDisplayConfig.h"

FhemStatusDisplayConfig::FhemStatusDisplayConfig()
{  
  // TODO: reset all members
}

void FhemStatusDisplayConfig::begin()
{
  Serial.println("");
  Serial.println("Initializing config.");

  // TODO: read from config file

  setWifiSSID("xxx");
  setWifiPSK("xxx");

  setMqttServer("xxx");
  setMqttStatusTopic("fhem/status/#");
  setMqttTestTopic("fhem/cmd/statusdisplay_01/test");  

  setNumberOfLeds(33);
  setLedDataPin(D2);
}

const String FhemStatusDisplayConfig::getWifiSSID() const
{
  return String(m_cfgWifiSSID);
}

bool FhemStatusDisplayConfig::setWifiSSID(String ssid)
{
  strcpy(m_cfgWifiSSID, ssid.c_str());
  return true;
}

const String FhemStatusDisplayConfig::getWifiPSK() const
{
  return String(m_cfgWifiPSK);
}

bool FhemStatusDisplayConfig::setWifiPSK(String psk)
{
  strcpy(m_cfgWifiPSK, psk.c_str());
  return true;
}

const char* FhemStatusDisplayConfig::getMqttServer() const
{
  return m_cfgMqttServer;
}

bool FhemStatusDisplayConfig::setMqttServer(const char* ip)
{
  strcpy(m_cfgMqttServer, ip);
  return true;
}

const char* FhemStatusDisplayConfig::getMqttStatusTopic() const
{
  return m_cfgMqttStatusTopic;
}

bool FhemStatusDisplayConfig::setMqttStatusTopic(const char* topic)
{
  strcpy(m_cfgMqttStatusTopic, topic);
  return true;
}

const char* FhemStatusDisplayConfig::getMqttTestTopic() const
{
  return m_cfgMqttTestTopic;
}

bool FhemStatusDisplayConfig::setMqttTestTopic(const char* topic)
{
  strcpy(m_cfgMqttTestTopic, topic);
  return true;
}

uint32_t FhemStatusDisplayConfig::getNumberOfLeds() const
{
  return m_cfgNumberOfLeds;
}

void FhemStatusDisplayConfig::setNumberOfLeds(uint32_t numberOfLeds)
{
  m_cfgNumberOfLeds = numberOfLeds;
}

uint32_t FhemStatusDisplayConfig::getLedDataPin() const
{
  return m_cfgLedDataPin;
}

void FhemStatusDisplayConfig::setLedDataPin(uint32 dataPin)
{
  m_cfgLedDataPin = dataPin;
}

int FhemStatusDisplayConfig::getLedNumber(String deviceName, deviceType deviceType)
{
  int number = -1;

  for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(deviceMap); i++)
  {
    if(deviceName.equals(deviceMap[i].name) && (deviceType == deviceMap[i].type))
    {
      number = deviceMap[i].ledNumber;
      break;
    }
  }

  return number;
}

int FhemStatusDisplayConfig::getColorMapIndex(deviceType deviceType, String msg)
{
  int index = -1;

  for(uint32_t i=0; i<NUMBER_OF_ELEMENTS(colorMap); i++)
  {
    if(msg.equals(colorMap[i].msg) && (deviceType == colorMap[i].type))
    {
      index = i;
      break;
    }
  }

  return index;
}

Led::Behavior FhemStatusDisplayConfig::getLedBehavior(int colorMapIndex)
{
  return colorMap[colorMapIndex].behavior;
}

Led::Color FhemStatusDisplayConfig::getLedColor(int colorMapIndex)
{
  return colorMap[colorMapIndex].color;
}

