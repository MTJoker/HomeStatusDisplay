#ifndef _WIFI_CONNECTOR_
#define _WIFI_CONNECTOR_

class WifiConnector
{
public:

  /*
   * Constructor. Supply SSID and PWD to the WLAN you want to connect
   */
  WifiConnector(const char* ssid, const char* pwd);

  /*
   * Connect to WLAN with credentials supplied in constructor.
   * ATTENTION: This function does not return until WLAN is connected!
   */
  void connect();

  /*
   * Check the connection to the WLAN. If it is not alive any more, it
   * is connected again. 
   * ATTENTION: This function does not return until WLAN is connected!
   */
  void checkConnection();
    
private:

    const char* m_wlanSSID;
    const char* m_wlanPwd;
};

#endif
