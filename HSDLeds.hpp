#pragma once

#include "HSDConfig.hpp"
#include <Adafruit_NeoPixel.h>

class HSDLeds
{
public:
  
  HSDLeds(const HSDConfig& config);
  ~HSDLeds();

  void begin();
  void update();
  void clear();
  
  void set(uint32_t ledNum, HSDConfig::Behavior behavior, HSDConfig::Color color);
  void setAll(HSDConfig::Behavior behavior, HSDConfig::Color color);

  HSDConfig::Color getColor(uint32_t ledNum) const;
  HSDConfig::Behavior getBehavior(uint32_t ledNum) const;

  void test(uint32_t type);

private:

  struct LedState
  {
    HSDConfig::Behavior behavior;
    HSDConfig::Color color;
  };
  
  static const uint32_t blinkOnTime = 500;
  static const uint32_t blinkOffTime = 500;
  static const uint32_t flashOnTime = 2000;
  static const uint32_t flashOffTime = 200;
  static const uint32_t flickerOnTime  = 100;
  static const uint32_t flickerOffTime  = 100;

  void handleBlink(unsigned long currentMillis);
  void handleFlash(unsigned long currentMillis);
  void handleFlicker(unsigned long currentMillis);

  void updateStripe();

  const HSDConfig& m_config;

  uint32_t m_numLeds;
  Adafruit_NeoPixel m_stripe;
  LedState* m_pLedState;
  
  bool m_blinkOn;
  bool m_flashOn;
  bool m_flickerOn;

  unsigned long m_previousMillisBlink;
  unsigned long m_previousMillisFlash;
  unsigned long m_previousMillisFlicker;
};

