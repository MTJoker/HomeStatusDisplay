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
    MODE_BLINKING,
    MODE_FLASHING
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
  void update();
  
private:
  
  static const uint32_t blinkOnTime = 500;
  static const uint32_t blinkOffTime = 500;
  static const uint32_t flashOnTime = 2000;
  static const uint32_t flashOffTime = 200;

  Adafruit_NeoPixel m_leds;
  
  int m_ledNumber;
  ledState m_ledState;
  bool m_isOn;
    
  uint32_t m_onTime;
  uint32_t m_offTime;

  unsigned long m_previousMillis;
};

#endif
