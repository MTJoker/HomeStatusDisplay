#pragma once

#include "HSDConfig.h"
#include "HSDLed.h"
#include <Adafruit_NeoPixel.h>

class HSDLeds
{
public:

  HSDLeds(const HSDConfig& config);
  ~HSDLeds();

  void begin();
  void update();
  void clear();
  void set(uint32_t ledNum, HSDLed::Behavior behavior, HSDLed::Color color);
  void setAll(HSDLed::Behavior behavior, HSDLed::Color color);
  void test(uint32_t type);

private:

  static const uint32_t blinkOnTime   = 500;
  static const uint32_t blinkOffTime  = 500;
  static const uint32_t flashOnTime  = 2000;
  static const uint32_t flashOffTime  = 200;

  void updateStripe();

  const HSDConfig& m_config;

  uint32_t m_numLeds;
  HSDLed* m_pLeds;
  Adafruit_NeoPixel m_stripe;
};

