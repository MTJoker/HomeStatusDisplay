#include "Heartbeat.h"
#include <Arduino.h>

Heartbeat::Heartbeat(int ledPin, long onTime, long offTime)
:
m_ledPin(ledPin),
m_onTime(onTime),
m_offTime(offTime),
m_ledState(LOW),
m_previousMillis(0)
{
  pinMode(ledPin, OUTPUT);
}

void Heartbeat::update()
{
  unsigned long currentMillis = millis();

  if((m_ledState == HIGH) && (currentMillis - m_previousMillis >= m_onTime))
  {
    m_ledState = LOW; 
    m_previousMillis = currentMillis; 
    digitalWrite(m_ledPin, m_ledState); 
  }
  else if ((m_ledState == LOW) && (currentMillis - m_previousMillis >= m_offTime))
  {
    m_ledState = HIGH;
    m_previousMillis = currentMillis;
    digitalWrite(m_ledPin, m_ledState); 
  }
}

