#pragma once

#include "Led.h"
#include <Adafruit_NeoPixel.h>

class StatusDisplayLeds
{
public:

  StatusDisplayLeds(uint32_t numLeds, uint8_t dataPin);
  ~StatusDisplayLeds();
  
  void update();
  void set(uint32_t ledNum, Led::Behavior behavior, Led::Color color);
  void clear();
  void test(uint32_t type);

private:

  static const uint32_t blinkOnTime = 500;
  static const uint32_t blinkOffTime = 500;
  static const uint32_t flashOnTime = 2000;
  static const uint32_t flashOffTime = 200;

  void updateStripe();

  uint32_t m_numLeds;
  Led* m_pLeds;
  Adafruit_NeoPixel m_stripe;
};

