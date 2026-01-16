#pragma once

#include <cstdint>

class HSDConfig;

class HSDWifi
{
  public:
    enum class State
    {
        NoConfig,   // no WLAN-config available
        Connecting, // STA connecting
        Connected,  // STA connected
        Failed,     // too many failed attempts
        AccessPoint // SoftAP active
    };

    explicit HSDWifi(const HSDConfig& config);

    void begin();
    void handle();

    bool isConnected() const;
    bool isAccessPointActive() const;
    State getState() const
    {
        return m_state;
    }

  private:
    void startSta();
    void startAp();
    void stopAp();

    void handleConnecting();
    void handleConnected();

  private:
    const HSDConfig& m_config;

    State m_state = State::NoConfig;

    unsigned long m_stateTimestamp = 0;
    uint8_t m_retryCount = 0;

    static constexpr unsigned long ConnectTimeoutMs = 15000;
    static constexpr uint8_t MaxRetries = 3;
};
