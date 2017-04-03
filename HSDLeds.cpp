#include "HSDLeds.h"

#define NUMBER_OF_ELEMENTS(array)  (sizeof(array)/sizeof(array[0]))

HSDLeds::HSDLeds(const HSDConfig& config)
:
m_config(config),
m_blinkOn(false),
m_flashOn(false),
m_flickerOn(false),
m_previousMillisBlink(0),
m_previousMillisFlash(0),
m_previousMillisFlicker(0)
{
}

HSDLeds::~HSDLeds()
{
  if(m_pLedState)
  {
    delete [] m_pLedState;
  }
}

void HSDLeds::begin()
{
  m_numLeds = m_config.getNumberOfLeds();
  m_pLedState = new LedState[m_numLeds];

  m_stripe.setPin(m_config.getLedDataPin());
  m_stripe.updateLength(m_numLeds);
  m_stripe.updateType(NEO_GRB + NEO_KHZ800);
  
  clear();
  m_stripe.begin();
}

void HSDLeds::set(uint32_t ledNum, HSDConfig::Behavior behavior, HSDConfig::Color color)
{ 
  if(ledNum < m_numLeds)
  {
    m_pLedState[ledNum].behavior = behavior;
    m_pLedState[ledNum].color = color;

    updateStripe();
  }
}

void HSDLeds::setAll(HSDConfig::Behavior behavior, HSDConfig::Color color)
{
  for(uint32_t i=0; i < m_numLeds; i++)
  {
    set(i, behavior, color);
  }

  updateStripe();
}

HSDConfig::Color HSDLeds::getColor(uint32_t ledNum) const
{
  HSDConfig::Color color = HSDConfig::NONE;

  if(ledNum < m_numLeds)
  {
    color = m_pLedState[ledNum].color;
  }
  
  return color;
}

HSDConfig::Behavior HSDLeds::getBehavior(uint32_t ledNum) const
{
  HSDConfig::Behavior behavior = HSDConfig::OFF;

  if(ledNum < m_numLeds)
  {
    behavior = m_pLedState[ledNum].behavior;
  }
  
  return behavior;
}

void HSDLeds::updateStripe()
{
  for(uint32_t i=0; i < m_numLeds; i++)
  {
    if( (m_pLedState[i].behavior == HSDConfig::ON) || 
        ( (m_pLedState[i].behavior == HSDConfig::BLINKING) && m_blinkOn ) ||
        ( (m_pLedState[i].behavior == HSDConfig::FLASHING) && m_flashOn ) ||
        ( (m_pLedState[i].behavior == HSDConfig::FLICKERING) && m_flickerOn ) )
    {
      m_stripe.setPixelColor(i, m_pLedState[i].color);
    }
    else
    {
      m_stripe.setPixelColor(i, HSDConfig::NONE);
    }
  }
  
  m_stripe.show();
}

void HSDLeds::clear()
{
  for(uint32_t i=0; i < m_numLeds; i++)
  {
    m_pLedState[i].behavior = HSDConfig::OFF;
    m_pLedState[i].color = HSDConfig::NONE;
  }
  
  updateStripe();
}

void HSDLeds::update()
{
  unsigned long currentMillis = millis();

  handleBlink(currentMillis);
  handleFlash(currentMillis);
  handleFlicker(currentMillis);

  updateStripe();
}

void HSDLeds::handleBlink(unsigned long currentMillis)
{
  if(!m_blinkOn && (currentMillis - m_previousMillisBlink >= blinkOnTime))
  {
    m_blinkOn = true;
    m_previousMillisBlink = currentMillis; 
  }
  else if (m_blinkOn && (currentMillis - m_previousMillisBlink >= blinkOffTime))
  {
    m_blinkOn = false;
    m_previousMillisBlink = currentMillis;
  } 
}

void HSDLeds::handleFlash(unsigned long currentMillis)
{
  if(!m_flashOn && (currentMillis - m_previousMillisFlash >= flashOnTime))
  {
    m_flashOn = true;
    m_previousMillisFlash = currentMillis; 
  }
  else if (m_flashOn && (currentMillis - m_previousMillisFlash >= flashOffTime))
  {
    m_flashOn = false;
    m_previousMillisFlash = currentMillis;
  } 
}

void HSDLeds::handleFlicker(unsigned long currentMillis)
{
  if(!m_flickerOn && (currentMillis - m_previousMillisFlicker >= flickerOnTime))
  {
    m_flickerOn = true;
    m_previousMillisFlicker = currentMillis; 
  }
  else if (m_flickerOn && (currentMillis - m_previousMillisFlicker >= flickerOffTime))
  {
    m_flickerOn = false;
    m_previousMillisFlicker = currentMillis;
  } 
}

void HSDLeds::test(uint32_t type)
{
  clear();

  if(type == 1) // left row on
  {
    for(uint32_t led = 0; led < m_numLeds/3; led++)
    {
      m_pLedState[led].behavior = HSDConfig::ON;
      m_pLedState[led].color = HSDConfig::GREEN;
    }
    updateStripe();
  }
  else if(type == 2)  // middle row on
  {
    for(uint32_t led = m_numLeds/3; led < m_numLeds/3*2; led++)
    {
      m_pLedState[led].behavior = HSDConfig::ON;
      m_pLedState[led].color = HSDConfig::GREEN;
    }
    updateStripe();
  }
  else if(type == 3)  // right row on
  {
    for(uint32_t led = m_numLeds/3*2; led < m_numLeds; led++)
    {
      m_pLedState[led].behavior = HSDConfig::ON;
      m_pLedState[led].color = HSDConfig::GREEN;
    }
    updateStripe();
  }
  else if(type == 4)  // all rows on
  {
    for(uint32_t led = 0; led < m_numLeds; led++)
    {
      m_pLedState[led].behavior = HSDConfig::ON;
      m_pLedState[led].color = HSDConfig::GREEN;
    }
    updateStripe();
  }
  else if(type == 5)
  {
    HSDConfig::Color colors[] = {HSDConfig::RED, HSDConfig::GREEN, HSDConfig::BLUE};

    for(uint32_t led = 0; led < m_numLeds/3; led++)
    {
      for(uint32_t colorIndex = 0; colorIndex < NUMBER_OF_ELEMENTS(colors); colorIndex++)
      {
        m_pLedState[led].behavior = HSDConfig::ON;
        m_pLedState[led].color = colors[colorIndex];
  
        m_pLedState[led + m_numLeds/3].behavior = HSDConfig::ON;
        m_pLedState[led + m_numLeds/3].color = colors[colorIndex];
  
        m_pLedState[led + m_numLeds/3*2].behavior = HSDConfig::ON;
        m_pLedState[led + m_numLeds/3*2].color = colors[colorIndex];

        updateStripe();
        delay(50);
      }

      m_pLedState[led].behavior = HSDConfig::OFF;
      m_pLedState[led].color = HSDConfig::NONE;

      m_pLedState[led + m_numLeds/3].behavior = HSDConfig::OFF;
      m_pLedState[led + m_numLeds/3].color = HSDConfig::NONE;

      m_pLedState[led + m_numLeds/3*2].behavior = HSDConfig::OFF;
      m_pLedState[led + m_numLeds/3*2].color = HSDConfig::NONE;
  
      updateStripe();
      delay(5);
    }
  }
}

