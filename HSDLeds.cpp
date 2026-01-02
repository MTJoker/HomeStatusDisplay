#include "HSDLeds.hpp"
#include <algorithm>

HSDLeds::HSDLeds(const HSDConfig& config)
    : m_config(config)
{
}

void HSDLeds::begin()
{
    m_numLeds = m_config.getNumberOfLeds();
    m_ledState = std::make_unique<LedState[]>(m_numLeds);

    m_stripe.setPin(m_config.getLedDataPin());
    m_stripe.updateLength(m_numLeds);
    m_stripe.updateType(NEO_GRB + NEO_KHZ800);
    m_stripe.setBrightness(m_config.getLedBrightness());

    m_stripe.begin();
    clear();
}

void HSDLeds::set(uint32_t ledNum, HSDConfig::Behavior behavior, HSDConfig::Color color)
{
    if(ledNum >= m_numLeds)
        return;

    m_ledState[ledNum].behavior = behavior;
    m_ledState[ledNum].color = color;
}

void HSDLeds::setAll(HSDConfig::Behavior behavior, HSDConfig::Color color)
{
    for(uint32_t i = 0; i < m_numLeds; ++i)
    {
        m_ledState[i].behavior = behavior;
        m_ledState[i].color = color;
    }
}

HSDConfig::Color HSDLeds::getColor(uint32_t ledNum) const
{
    return (ledNum < m_numLeds) ? m_ledState[ledNum].color : HSDConfig::NONE;
}

HSDConfig::Behavior HSDLeds::getBehavior(uint32_t ledNum) const
{
    return (ledNum < m_numLeds) ? m_ledState[ledNum].behavior : HSDConfig::OFF;
}

void HSDLeds::clear()
{
    for(uint32_t i = 0; i < m_numLeds; ++i)
    {
        m_ledState[i] = {};
    }

    updateStripe();
}

void HSDLeds::update()
{
    const unsigned long now = millis();

    handleBlink(now);
    handleFlash(now);
    handleFlicker(now);

    updateStripe();
}

void HSDLeds::updateStripe()
{
    for(uint32_t i = 0; i < m_numLeds; ++i)
    {
        const auto& led = m_ledState[i];

        const bool on =
            led.behavior == HSDConfig::ON ||
            (led.behavior == HSDConfig::BLINKING && m_blinkOn) ||
            (led.behavior == HSDConfig::FLASHING && m_flashOn) ||
            (led.behavior == HSDConfig::FLICKERING && m_flickerOn);

        m_stripe.setPixelColor(i, on ? led.color : HSDConfig::NONE);
    }

    m_stripe.show();
}

void HSDLeds::handleBlink(unsigned long now)
{
    if((now - m_prevBlink) >= (m_blinkOn ? BLINK_OFF_TIME : BLINK_ON_TIME))
    {
        m_blinkOn = !m_blinkOn;
        m_prevBlink = now;
    }
}

void HSDLeds::handleFlash(unsigned long now)
{
    if((now - m_prevFlash) >= (m_flashOn ? FLASH_OFF_TIME : FLASH_ON_TIME))
    {
        m_flashOn = !m_flashOn;
        m_prevFlash = now;
    }
}

void HSDLeds::handleFlicker(unsigned long now)
{
    if((now - m_prevFlicker) >= (m_flickerOn ? FLICKER_OFF_TIME : FLICKER_ON_TIME))
    {
        m_flickerOn = !m_flickerOn;
        m_prevFlicker = now;
    }
}

void HSDLeds::test(uint32_t type)
{
    clear();

    auto setRange = [&](uint32_t from, uint32_t to, HSDConfig::Color color)
    {
        for(uint32_t led = from; led < to; ++led)
        {
            m_ledState[led].behavior = HSDConfig::ON;
            m_ledState[led].color = color;
        }
        updateStripe();
    };

    const uint32_t third = m_numLeds / 3;

    switch(type)
    {
    case 1: // left row
        setRange(0, third, HSDConfig::GREEN);
        break;

    case 2: // middle row
        setRange(third, third * 2, HSDConfig::GREEN);
        break;

    case 3: // right row
        setRange(third * 2, m_numLeds, HSDConfig::GREEN);
        break;

    case 4: // all rows
        setRange(0, m_numLeds, HSDConfig::GREEN);
        break;

    case 5: // color sweep
    {
        const HSDConfig::Color colors[] =
            {
                HSDConfig::RED,
                HSDConfig::GREEN,
                HSDConfig::BLUE};

        for(uint32_t led = 0; led < third; ++led)
        {
            for(HSDConfig::Color c : colors)
            {
                for(uint32_t col = 0; col < 3; ++col)
                {
                    uint32_t idx = led + col * third;
                    m_ledState[idx].behavior = HSDConfig::ON;
                    m_ledState[idx].color = c;
                }

                updateStripe();
                delay(50);
            }

            for(uint32_t col = 0; col < 3; ++col)
            {
                uint32_t idx = led + col * third;
                m_ledState[idx].behavior = HSDConfig::OFF;
                m_ledState[idx].color = HSDConfig::NONE;
            }

            updateStripe();
            delay(5);
        }
        break;
    }

    default:
        // unknown test pattern
        break;
    }
}
