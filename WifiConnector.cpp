#include "WifiConnector.h"
#include <ESP8266WiFi.h>

WifiConnector::WifiConnector(const FhemStatusDisplayConfig& config)
:
m_config(config)
{
}

void WifiConnector::connect()
{
  WiFi.begin(m_config.getWifiSSID(), m_config.getWifiPSK());

  Serial.println("");
  Serial.print("Starting Wifi connection to ");
  Serial.print(m_config.getWifiSSID());

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.print("WiFi connected with IP ");
  Serial.print(WiFi.localIP());
  Serial.println(".");
}

void WifiConnector::checkConnection()
{
  if(WiFi.status() != WL_CONNECTED) 
  {
    connect();
  }
}

