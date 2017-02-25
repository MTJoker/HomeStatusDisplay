#include "StatusDisplayLeds.h"
#include "FhemStatusDisplayTypes.h"

StatusDisplayLeds::StatusDisplayLeds(const FhemStatusDisplayConfig& config)
:
m_config(config)
{

}

StatusDisplayLeds::~StatusDisplayLeds()
{
  if(m_pLeds)
  {
    delete [] m_pLeds;
  }
}

void StatusDisplayLeds::begin()
{
  m_numLeds = m_config.getNumberOfLeds();
  m_pLeds = new Led[m_numLeds];

  m_stripe.setPin(m_config.getLedDataPin());
  m_stripe.updateLength(m_numLeds);
  m_stripe.updateType(NEO_GRB + NEO_KHZ800);
  
  clear();
  m_stripe.begin();
}

void StatusDisplayLeds::set(uint32_t ledNum, Led::Behavior behavior, Led::Color color)
{ 
  if(ledNum < m_numLeds)
  {
    if(behavior == Led::ON)
    {
      m_pLeds[ledNum].setOn(color);
    }
    else if(behavior == Led::OFF)
    {
      m_pLeds[ledNum].setOff();
    }
    else if(behavior == Led::BLINKING)
    {
      m_pLeds[ledNum].setBlinking(color);
    }
    else if(behavior == Led::FLASHING)
    {
      m_pLeds[ledNum].setFlashing(color);
    }

    updateStripe();
  }
}

void StatusDisplayLeds::updateStripe()
{
  for(uint32_t i=0; i < m_numLeds; i++)
  {
    m_stripe.setPixelColor(i, m_pLeds[i].getColor());
  }
  
  m_stripe.show();
}

void StatusDisplayLeds::clear()
{
  for(uint32_t i=0; i < m_numLeds; i++)
  {
    m_pLeds[i].setOff();
  }
  
  updateStripe();
}

void StatusDisplayLeds::update()
{
  for(uint32_t i=0; i < m_numLeds; i++)
  {
    m_pLeds[i].update();
  }
  
  updateStripe();
}

void StatusDisplayLeds::test(uint32_t type)
{
  clear();

  if(type == 1) // left row on
  {
    for(uint32_t led = 0; led < m_numLeds/3; led++)
    {
      m_pLeds[led].setOn(Led::GREEN);
    }
    updateStripe();
  }
  else if(type == 2)  // middle row on
  {
    for(uint32_t led = m_numLeds/3; led < m_numLeds/3*2; led++)
    {
      m_pLeds[led].setOn(Led::GREEN);
    }
    updateStripe();
  }
  else if(type == 3)  // right row on
  {
    for(uint32_t led = m_numLeds/3*2; led < m_numLeds; led++)
    {
      m_pLeds[led].setOn(Led::GREEN);
    }
    updateStripe();
  }
  else if(type == 4)
  {
    Led::Color colors[] = {Led::RED, Led::GREEN, Led::BLUE};

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

