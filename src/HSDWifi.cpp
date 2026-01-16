#include "HSDWifi.hpp"
#include "HSDConfig.hpp"
#include <ESP8266WiFi.h>

inline constexpr char SoftApSsid[] = "HomeStatusDisplay";
inline constexpr char SoftApPsk[] = "homestatusdisplay";

HSDWifi::HSDWifi(const HSDConfig& config)
    : m_config(config)
{
}

void HSDWifi::begin()
{
    WiFi.persistent(false);
    WiFi.setAutoReconnect(false);

    if(!m_config.hasWifiConfig())
    {
        Serial.println(F("No WiFi config available."));

        m_state = State::NoConfig;
        startAp();
        return;
    }

    startSta();
}

void HSDWifi::handle()
{
    switch(m_state)
    {
    case State::Connecting:
        handleConnecting();
        break;

    case State::Connected:
        handleConnected();
        break;

    case State::NoConfig:
    case State::Failed:
        if(m_state != State::AccessPoint)
        {
            startAp();
        }
        break;

    case State::AccessPoint:
        // wait for new config
        break;
    }
}

bool HSDWifi::isConnected() const
{
    return WiFi.status() == WL_CONNECTED;
}

bool HSDWifi::isAccessPointActive() const
{
    return m_state == State::AccessPoint;
}

void HSDWifi::startSta()
{
    stopAp();

    Serial.print(F("Starting WiFi connection to "));
    Serial.print(m_config.getWifiSSID());
    Serial.println(F("..."));

    WiFi.mode(WIFI_STA);
    WiFi.begin(m_config.getWifiSSID(), m_config.getWifiPSK());

    m_stateTimestamp = millis();
    m_state = State::Connecting;
}

void HSDWifi::handleConnecting()
{
    if(WiFi.status() == WL_CONNECTED)
    {
        Serial.print(F("WiFi connected with IP "));
        Serial.print(WiFi.localIP());
        Serial.println(F("."));

        m_state = State::Connected;
        m_retryCount = 0;
        return;
    }

    if(millis() - m_stateTimestamp > ConnectTimeoutMs)
    {
        ++m_retryCount;

        if(m_retryCount >= MaxRetries)
        {
            m_state = State::Failed;
        }
        else
        {
            WiFi.disconnect();
            startSta();
        }
    }
}

void HSDWifi::handleConnected()
{
    if(WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("WiFi connection lost."));

        startSta();
    }
}

void HSDWifi::startAp()
{
    Serial.println(F("Starting access point."));

    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);

    if(WiFi.softAP(SoftApSsid, SoftApPsk))
    {
        Serial.print(F("AccessPoint SSID is "));
        Serial.println(SoftApSsid);
        Serial.print(F("IP: "));
        Serial.println(WiFi.softAPIP());

        m_state = State::AccessPoint;
    }
    else
    {
        Serial.println(F("Error starting access point."));
    }
}

void HSDWifi::stopAp()
{
    if(WiFi.getMode() & WIFI_AP)
    {
        WiFi.softAPdisconnect(true);
    }
}
