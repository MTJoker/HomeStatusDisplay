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

    static constexpr int MaxConnectRetries = 100;
    static constexpr unsigned long RetryDelayMs = 500;

    const HSDConfig& m_config;

    bool m_connectionFailed = false;
    int m_retryCount = 0;
    unsigned long m_millisLastConnectTry = 0;

    bool m_accessPointActive = false;
    bool m_lastConnectStatus = false;
};
