#include "FhemStatusDisplayWifi.h"
#include <ESP8266WiFi.h>

#define SOFT_AP_SSID  (F("StatusDisplay"))
#define SOFT_AP_PSK   (F("statusdisplay"))

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
  Serial.print(F("Starting Wifi connection to "));
  Serial.print(m_config.getWifiSSID());

  WiFi.mode(WIFI_STA);
  WiFi.begin(m_config.getWifiSSID(), m_config.getWifiPSK());
  
  uint32_t timeout = 0;
  while( (WiFi.status() != WL_CONNECTED) && (timeout < 20) ) 
  {
    delay(500);
    Serial.print(F("."));
    timeout++;
  }
  Serial.println(F(""));
  
  if(WiFi.status() == WL_CONNECTED)
  {
    success = true; 
    Serial.print(F("WiFi connected with IP "));
    Serial.print(WiFi.localIP());
    Serial.println(F("."));
  }  
  else
  {
    Serial.println(F("Failed to connect WiFi."));
  }

  return success;
}

void FhemStatusDisplayWifi::startAccessPoint()
{
  Serial.println(F(""));
  Serial.println(F("Starting access point."));

  WiFi.mode(WIFI_AP);
  if(WiFi.softAP(String(SOFT_AP_SSID).c_str(), String(SOFT_AP_PSK).c_str()))
  {
    IPAddress ip = WiFi.softAPIP();
  
    Serial.print(F("AccessPoint SSID is ")); Serial.print(SOFT_AP_SSID); Serial.println(ip);
  }
  else
  {
    Serial.println(F("Error starting access point."));
  }
}

