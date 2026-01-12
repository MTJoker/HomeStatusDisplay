#include "HSDWifi.hpp"
#include <ESP8266WiFi.h>

inline constexpr char SoftApSsid[] = "StatusDisplay";
inline constexpr char SoftApPsk[] = "statusdisplay";

HSDWifi::HSDWifi(const HSDConfig& config)
    : m_config(config)
{
}

void HSDWifi::begin()
{
    WiFi.persistent(false);
}

void HSDWifi::handleConnection()
{
    const bool isConnected = connected();

    if(isConnected != m_lastConnectStatus)
    {
        if(isConnected)
        {
            Serial.print(F("WiFi connected with IP "));
            Serial.print(WiFi.localIP());
            Serial.println(F("."));

            m_retryCount = 0;
        }
        else
        {
            Serial.println(F("WiFi connection lost."));
        }

        m_lastConnectStatus = isConnected;
    }

    if(isConnected || m_accessPointActive)
    {
        return;
    }

    if(m_connectionFailed)
    {
        startAccessPoint();
        return;
    }

    const unsigned long now = millis();
    if((now - m_millisLastConnectTry) < RetryDelayMs)
        return;

    m_millisLastConnectTry = now;

    if(m_retryCount == 0)
    {
        Serial.print(F("Starting WiFi connection to "));
        Serial.print(m_config.getWifiSSID());
        Serial.println(F("..."));

        WiFi.mode(WIFI_STA);
        WiFi.begin(m_config.getWifiSSID(), m_config.getWifiPSK());
    }

    if(++m_retryCount >= MaxConnectRetries)
    {
        Serial.println(F("Failed to connect WiFi."));
        m_connectionFailed = true;
    }
}

bool HSDWifi::connected() const
{
    return WiFi.status() == WL_CONNECTED;
}

void HSDWifi::startAccessPoint()
{
    Serial.println();
    Serial.println(F("Starting access point."));

    WiFi.mode(WIFI_AP);

    if(WiFi.softAP(SoftApSsid, SoftApPsk))
    {
        Serial.print(F("AccessPoint SSID is "));
        Serial.println(SoftApSsid);
        Serial.print(F("IP: "));
        Serial.println(WiFi.softAPIP());

        m_accessPointActive = true;
    }
    else
    {
        Serial.println(F("Error starting access point."));
    }
}
