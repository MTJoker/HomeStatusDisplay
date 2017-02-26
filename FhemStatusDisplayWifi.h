#pragma once

#include "FhemStatusDisplayConfig.h"

class FhemStatusDisplayWifi
{
public:

  FhemStatusDisplayWifi(const FhemStatusDisplayConfig& config);

  void begin();
  bool connected();

private:

  bool startWifi();
  void startAccessPoint();

  const FhemStatusDisplayConfig& m_config;
};

