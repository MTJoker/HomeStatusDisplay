#include "WifiConnector.h"
#include <ESP8266WiFi.h>

WifiConnector::WifiConnector(const char* ssid, const char* pwd)
:
m_wlanSSID(ssid),
m_wlanPwd(pwd)
{
}

void WifiConnector::connect()
{
  WiFi.begin(m_wlanSSID, m_wlanPwd);

  Serial.println("");
  Serial.print("Connecting to WLAN ");
  Serial.print(m_wlanSSID);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.print(" connected, got IP address ");
  Serial.println(WiFi.localIP());
}