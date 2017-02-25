#pragma once

#include <Arduino.h>

class Led
{
public:

  enum Behavior
  {
    OFF,
    ON,
    BLINKING,
    FLASHING
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

  Led();
  void setOn(Color color);
  void setOff();
  void setBlinking(Color color);
  void setFlashing(Color color);
  void update();
  Color getColor();
  
private:

  static const uint32_t blinkOnTime = 500;
  static const uint32_t blinkOffTime = 500;
  static const uint32_t flashOnTime = 2000;
  static const uint32_t flashOffTime = 200;
  
  Behavior m_behavior;
  Color m_color;
  bool m_isOn;
  uint32_t m_onTime;
  uint32_t m_offTime;
  unsigned long m_previousMillis;
};

