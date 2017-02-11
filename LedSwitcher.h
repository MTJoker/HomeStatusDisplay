#ifndef _LED_SWITCHER_H_
#define _LED_SWITCHER_H_

#include <Adafruit_NeoPixel.h>

class LedSwitcher
{
public:

  enum ledMode
  {
    MODE_OFF,
    MODE_ON,
    MODE_BLINKING
  };

  enum ledColor
  {
    COLOR_OFF    = 0x000000,
    COLOR_RED    = 0x0F0000,
    COLOR_GREEN  = 0x000F00,
    COLOR_BLUE   = 0x00000F,
    COLOR_YELLOW = 0x0F0F00,
    COLOR_WHITE  = 0x0F0F0F
  };

  struct ledState
  {
    ledColor color;
    ledMode mode;
  };

  LedSwitcher();

  void init(int ledNumber, Adafruit_NeoPixel& leds);
  void set(ledState& state);
  void setBlinkTiming(uint32_t onTime, uint32_t offTime);
  void update();
  
private:

  static const uint32_t defaultBlinkOnTime = 500;
  static const uint32_t defaultBlinkOffTime = 500;

  Adafruit_NeoPixel m_leds;
  
  int m_ledNumber;
  ledState m_ledState;
  bool m_isOn;
    
  uint32_t m_blinkOnTime;
  uint32_t m_blinkOffTime;

  unsigned long m_previousMillis;
};

#endif
