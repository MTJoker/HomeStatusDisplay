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

  HSDLed::Color getColor(uint32_t ledNum) const;
  HSDLed::Behavior getBehavior(uint32_t ledNum) const;

  void test(uint32_t type);

private:

  void updateStripe();

  const HSDConfig& m_config;

  uint32_t m_numLeds;
  HSDLed* m_pLeds;
  Adafruit_NeoPixel m_stripe;
};

