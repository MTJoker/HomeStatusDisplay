#pragma once

#include "FhemStatusDisplayConfig.h"

class FhemStatusDisplayWifi
{
public:

  FhemStatusDisplayWifi(const FhemStatusDisplayConfig& config);

  void begin();
  void handleConnection();
  bool connected();

private:

  void startAccessPoint();

  const FhemStatusDisplayConfig& m_config;
  bool m_connectFailure;
  int m_maxConnectRetries;
  int m_numConnectRetriesDone;
  int m_retryDelay;
  unsigned long m_millisLastConnectTry;
};

