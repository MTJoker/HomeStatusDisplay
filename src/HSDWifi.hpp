#pragma once

#include "HSDConfig.hpp"

class HSDWifi
{
  public:
    explicit HSDWifi(const HSDConfig& config);

    void begin();
    void handleConnection();
    bool connected() const;

  private:
    void startAccessPoint();

    static constexpr int MAX_CONNECT_RETRIES = 100;
    static constexpr unsigned long RETRY_DELAY_MS = 500;

    const HSDConfig& m_config;

    bool m_connectionFailed = false;
    int m_retryCount = 0;
    unsigned long m_millisLastConnectTry = 0;

    bool m_accessPointActive = false;
    bool m_lastConnectStatus = false;
};
