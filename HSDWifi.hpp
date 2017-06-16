#pragma once

#include "HSDConfig.hpp"

class HSDWifi
{
public:

  HSDWifi(const HSDConfig& config);

  void begin();
  void handleConnection();
  bool connected();

private:

  void startAccessPoint();

  const HSDConfig& m_config;
  bool m_connectFailure;
  int m_maxConnectRetries;
  int m_numConnectRetriesDone;
  int m_retryDelay;
  unsigned long m_millisLastConnectTry;
  bool m_accessPointActive;
  bool m_lastConnectStatus;
};

