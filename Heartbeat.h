#ifndef _HEARTBEAT_H
#define _HEARTBEAT_H

/**
 * Simple class to realize a "heartbeat" - that is a led which blinks in
 * a given interval to visualize the system is still working.
 */

class Heartbeat
{
public:

  Heartbeat(int ledPin, long onTime, long offTime);

  void update();
  void changeTiming(long onTime, long offTime);
  
private:

  int m_ledPin;
  long m_onTime;
  long m_offTime;

  int m_ledState;
  unsigned long m_previousMillis;
};

#endif
