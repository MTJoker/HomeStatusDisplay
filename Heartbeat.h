#ifndef _HEARTBEAT_H
#define _HEARTBEAT_H

class Heartbeat
{
public:

  Heartbeat(int ledPin, long onTime, long offTime);

  void update();
  
private:

  int m_ledPin;
  long m_onTime;
  long m_offTime;

  int m_ledState;
  unsigned long m_previousMillis;
};

#endif
