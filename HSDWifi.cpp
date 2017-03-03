#include "HSDWifi.h"
#include <ESP8266WiFi.h>

#define SOFT_AP_SSID  (F("StatusDisplay"))
#define SOFT_AP_PSK   (F("statusdisplay"))

HSDWifi::HSDWifi(const HSDConfig& config)
:
m_config(config),
m_connectFailure(false),
m_maxConnectRetries(20),
m_numConnectRetriesDone(0),
m_retryDelay(500),
m_millisLastConnectTry(0)
{
  
}

void HSDWifi::begin()
{
  // nothing to do right now
}

void HSDWifi::handleConnection()
{
  if(!connected()) // TODO: check access point running
  {  
    if(m_connectFailure)
    {
      startAccessPoint();
    }
    else
    {
      unsigned long currentMillis = millis();
      
      if( (currentMillis - m_millisLastConnectTry) >= m_retryDelay)
      {
        m_millisLastConnectTry = currentMillis; 
      
        if(m_numConnectRetriesDone == 0)
        {
          Serial.print(F("Starting Wifi connection to "));
          Serial.println(m_config.getWifiSSID());
      
          WiFi.mode(WIFI_STA);
          WiFi.begin(m_config.getWifiSSID(), m_config.getWifiPSK());
      
          m_numConnectRetriesDone++;
        }
        else if(m_numConnectRetriesDone < m_maxConnectRetries)
        {
          if(WiFi.status() != WL_CONNECTED)
          {
            m_numConnectRetriesDone++;
          }
          else
          {
            Serial.print(F("WiFi connected with IP "));
            Serial.print(WiFi.localIP());
            Serial.println(F("."));
      
            m_numConnectRetriesDone = 0;
          }
        }
        else
        {
          Serial.println(F("Failed to connect WiFi."));
          
          m_connectFailure = true;
        }   
      }   
    }
  }
}

bool HSDWifi::connected()
{
  return (WiFi.status() == WL_CONNECTED);
}

void HSDWifi::startAccessPoint()
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

