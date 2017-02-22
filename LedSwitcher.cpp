#include "LedSwitcher.h"
#include <Arduino.h>

LedSwitcher::LedSwitcher()
:
m_ledNumber(0),
m_isOn(false),
m_onTime(blinkOnTime),
m_offTime(blinkOffTime),
m_previousMillis(0)
{
  m_ledState.color = COLOR_OFF;
  m_ledState.mode = MODE_OFF;
}

void LedSwitcher::update()
{
    if( (m_ledState.mode == MODE_BLINKING) || (m_ledState.mode == MODE_FLASHING) )
    {
      // check to see if it's time to change the state of the LED
      unsigned long currentMillis = millis();
   
      if(!m_isOn && (currentMillis - m_previousMillis >= m_onTime))
      {
  	      m_isOn = true;
          m_previousMillis = currentMillis; 
          m_leds.setPixelColor(m_ledNumber, m_ledState.color);
          m_leds.show();
      }
      else if (m_isOn && (currentMillis - m_previousMillis >= m_offTime))
      {
          m_isOn = false;
          m_previousMillis = currentMillis;
          m_leds.setPixelColor(m_ledNumber, COLOR_OFF);
          m_leds.show();
      }
    }
}

void LedSwitcher::init(int ledNumber, Adafruit_NeoPixel& leds)
{
  m_ledNumber = ledNumber;
  m_leds = leds;
}

void LedSwitcher::set(ledState& state)
{
  m_ledState = state;

  if(m_ledState.mode == MODE_ON)
  {
    Serial.println("Set led number " + String(m_ledNumber) + " to ON with color " + String(m_ledState.color, HEX));
      
    m_isOn = true;
    m_leds.setPixelColor(m_ledNumber, m_ledState.color); 
    m_leds.show();   
  }
  else if(m_ledState.mode == MODE_OFF)
  {
    Serial.println("Set led number " + String(m_ledNumber) + " to OFF");
    
    m_isOn = false;
    m_leds.setPixelColor(m_ledNumber, COLOR_OFF);
    m_leds.show();
  }
  else if(m_ledState.mode == MODE_BLINKING)
  {
    Serial.println("Set led number " + String(m_ledNumber) + " to BLINKING with color " + String(m_ledState.color, HEX));
        
    m_isOn = false;
    m_previousMillis = 0;
    m_onTime = blinkOnTime;
    m_offTime = blinkOffTime;
    m_leds.setPixelColor(m_ledNumber, COLOR_OFF);
    m_leds.show();
  }
  else if(m_ledState.mode == MODE_FLASHING)
  {
    Serial.println("Set led number " + String(m_ledNumber) + " to FLASHING with color " + String(m_ledState.color, HEX));
        
    m_isOn = false;
    m_previousMillis = 0;
    m_onTime = flashOnTime;
    m_offTime = flashOffTime;
    m_leds.setPixelColor(m_ledNumber, COLOR_OFF);
    m_leds.show();
  }
}

