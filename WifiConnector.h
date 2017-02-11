#ifndef _WIFI_CONNECTOR_
#define _WIFI_CONNECTOR_

class WifiConnector
{
public:

    WifiConnector(const char* ssid, const char* pwd);
    void connect();

private:

    const char* m_wlanSSID;
    const char* m_wlanPwd;
};

#endif