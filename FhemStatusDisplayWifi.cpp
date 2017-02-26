#include "FhemStatusDisplayWifi.h"
#include <ESP8266WiFi.h>

const char* softApSsid = "StatusDisplay";
const char* softApPw = "statusdisplay";

FhemStatusDisplayWifi::FhemStatusDisplayWifi(const FhemStatusDisplayConfig& config)
:
m_config(config)
{
  
}

void FhemStatusDisplayWifi::begin()
{
  if(!startWifi())
  {
    startAccessPoint();
  }
}

bool FhemStatusDisplayWifi::connected()
{
  return (WiFi.status() == WL_CONNECTED);
}

bool FhemStatusDisplayWifi::startWifi()
{
  bool success = false;
  
  Serial.println("");
  Serial.print("Starting Wifi connection to ");
  Serial.print(m_config.getWifiSSID());

  WiFi.mode(WIFI_STA);
  WiFi.begin(m_config.getWifiSSID(), m_config.getWifiPSK());
  
  uint32_t timeout = 0;
  while( (WiFi.status() != WL_CONNECTED) && (timeout < 20) ) 
  {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  Serial.println("");
  
  if(WiFi.status() == WL_CONNECTED)
  {
    success = true; 
    Serial.print("WiFi connected with IP ");
    Serial.print(WiFi.localIP());
    Serial.println(".");
  }  
  else
  {
    Serial.println("Failed to connect WiFi.");
  }

  return success;
}

void FhemStatusDisplayWifi::startAccessPoint()
{
  Serial.println("");
  Serial.println("Starting access point.");

  WiFi.mode(WIFI_AP);
  if(WiFi.softAP(softApSsid, softApPw))
  {
    IPAddress ip = WiFi.softAPIP();
  
    Serial.printf("AccessPoint SSID is %s and IP is ", softApSsid);
    Serial.println(ip);
  }
  else
  {
    Serial.println("Error starting access point.");
  }
}
