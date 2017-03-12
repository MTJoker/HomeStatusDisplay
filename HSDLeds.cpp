#include "HSDLeds.h"

#define NUMBER_OF_ELEMENTS(array)  (sizeof(array)/sizeof(array[0]))

HSDLeds::HSDLeds(const HSDConfig& config)
:
m_config(config)
{

}

HSDLeds::~HSDLeds()
{
  if(m_pLeds)
  {
    delete [] m_pLeds;
  }
}

void HSDLeds::begin()
{
  m_numLeds = m_config.getNumberOfLeds();
  m_pLeds = new HSDLed[m_numLeds];

  m_stripe.setPin(m_config.getLedDataPin());
  m_stripe.updateLength(m_numLeds);
  m_stripe.updateType(NEO_GRB + NEO_KHZ800);
  
  clear();
  m_stripe.begin();
}

void HSDLeds::set(uint32_t ledNum, HSDLed::Behavior behavior, HSDLed::Color color)
{ 
  if(ledNum < m_numLeds)
  {
    if(behavior == HSDLed::ON)
    {
      m_pLeds[ledNum].setOn(color);
    }
    else if(behavior == HSDLed::OFF)
    {
      m_pLeds[ledNum].setOff();
    }
    else if(behavior == HSDLed::BLINKING)
    {
      m_pLeds[ledNum].setBlinking(color);
    }
    else if(behavior == HSDLed::FLASHING)
    {
      m_pLeds[ledNum].setFlashing(color);
    }
    else if(behavior == HSDLed::FLICKERING)
    {
      m_pLeds[ledNum].setFlickering(color);
    }

    updateStripe();
  }
}

void HSDLeds::setAll(HSDLed::Behavior behavior, HSDLed::Color color)
{
  for(uint32_t i=0; i < m_numLeds; i++)
  {
    set(i, behavior, color);
  }

  updateStripe();
}

HSDLed::Color HSDLeds::getColor(uint32_t ledNum) const
{
  HSDLed::Color color = HSDLed::NONE;

  if(ledNum < m_numLeds)
  {
    color = m_pLeds[ledNum].getColor();
  }
  
  return color;
}

HSDLed::Behavior HSDLeds::getBehavior(uint32_t ledNum) const
{
  HSDLed::Behavior behavior = HSDLed::OFF;

  if(ledNum < m_numLeds)
  {
    behavior = m_pLeds[ledNum].getBehavior();
  }
  
  return behavior;
}

void HSDLeds::updateStripe()
{
  for(uint32_t i=0; i < m_numLeds; i++)
  {
    if(!m_pLeds[i].isOn())
    {
      m_stripe.setPixelColor(i, HSDLed::NONE);
    }
    else
    {
      m_stripe.setPixelColor(i, m_pLeds[i].getColor());
    }
  }
  
  m_stripe.show();
}

void HSDLeds::clear()
{
  for(uint32_t i=0; i < m_numLeds; i++)
  {
    m_pLeds[i].setOff();
  }
  
  updateStripe();
}

void HSDLeds::update()
{
  for(uint32_t i=0; i < m_numLeds; i++)
  {
    m_pLeds[i].update();
  }
  
  updateStripe();
}

void HSDLeds::test(uint32_t type)
{
  clear();

  if(type == 1) // left row on
  {
    for(uint32_t led = 0; led < m_numLeds/3; led++)
    {
      m_pLeds[led].setOn(HSDLed::GREEN);
    }
    updateStripe();
  }
  else if(type == 2)  // middle row on
  {
    for(uint32_t led = m_numLeds/3; led < m_numLeds/3*2; led++)
    {
      m_pLeds[led].setOn(HSDLed::GREEN);
    }
    updateStripe();
  }
  else if(type == 3)  // right row on
  {
    for(uint32_t led = m_numLeds/3*2; led < m_numLeds; led++)
    {
      m_pLeds[led].setOn(HSDLed::GREEN);
    }
    updateStripe();
  }
  else if(type == 4)  // all rows on
  {
    for(uint32_t led = 0; led < m_numLeds; led++)
    {
      m_pLeds[led].setOn(HSDLed::GREEN);
    }
    updateStripe();
  }
  else if(type == 5)
  {
    HSDLed::Color colors[] = {HSDLed::RED, HSDLed::GREEN, HSDLed::BLUE};

    for(uint32_t led = 0; led < m_numLeds/3; led++)
    {
      for(uint32_t colorIndex = 0; colorIndex < NUMBER_OF_ELEMENTS(colors); colorIndex++)
      {
        m_pLeds[led].setOn(colors[colorIndex]);
        m_pLeds[led + m_numLeds/3].setOn(colors[colorIndex]);
        m_pLeds[led + m_numLeds/3*2].setOn(colors[colorIndex]);
        updateStripe();
        delay(50);
      }
  
      m_pLeds[led].setOff();
      m_pLeds[led + m_numLeds/3].setOff();
      m_pLeds[led + m_numLeds/3*2].setOff();
      updateStripe();
      delay(5);
    }
  }
}

