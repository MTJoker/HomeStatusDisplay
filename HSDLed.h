#pragma once

#include <Arduino.h>

class HSDLed
{
public:

  enum Behavior
  {
    OFF,
    ON,
    BLINKING,
    FLASHING,
    FLICKERING
  };

  enum Color
  {
    NONE   = 0x000000,
    RED    = 0x0F0000,
    GREEN  = 0x000F00,
    BLUE   = 0x00000F,
    YELLOW = 0x0F0F00,
    WHITE  = 0x0F0F0F
  };

  HSDLed();
  void setOn(Color color);
  void setOff();
  void setBlinking(Color color);
  void setFlashing(Color color);
  void setFlickering(Color color);
  void update();
  Color getColor();
  
private:

  static const uint32_t blinkOnTime = 500;
  static const uint32_t blinkOffTime = 500;
  static const uint32_t flashOnTime = 2000;
  static const uint32_t flashOffTime = 200;
  static const uint32_t flickerOnTime  = 100;
  static const uint32_t flickerOffTime  = 100;
  
  Behavior m_behavior;
  Color m_color;
  bool m_isOn;
  uint32_t m_onTime;
  uint32_t m_offTime;
  unsigned long m_previousMillis;
};

