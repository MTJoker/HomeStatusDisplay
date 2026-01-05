#include "HSDLeds.hpp"
#include <algorithm>

HSDLeds::HSDLeds(const HSDConfig& config)
    : m_config(config)
{
}

void HSDLeds::begin()
{
    auto numLeds = m_config.getNumberOfLeds();

    m_ledState.resize(numLeds);

    auto pin = m_config.getLedDataPin();
    auto type = m_config.getLedType() == 0 ? NEO_GRB : NEO_GRBW;

    m_stripe = std::make_unique<Adafruit_NeoPixel>(m_ledState.size(), pin, type);

    m_stripe->setBrightness(m_config.getLedBrightness());
    m_stripe->begin();

    clear();
    updateStripe();
}

void HSDLeds::set(uint32_t ledNum, Behavior behavior, Color color)
{
    if(ledNum >= m_ledState.size())
    {
        return;
    }

    m_ledState.at(ledNum).behavior = behavior;
    m_ledState.at(ledNum).color = color;
}

void HSDLeds::setAll(Behavior behavior, Color color)
{
    std::fill(
        m_ledState.begin(),
        m_ledState.end(),
        LedState{behavior, color});
}

Color HSDLeds::getColor(uint32_t ledNum) const
{
    return (ledNum < m_ledState.size()) ? m_ledState.at(ledNum).color : Color::None;
}

Behavior HSDLeds::getBehavior(uint32_t ledNum) const
{
    return (ledNum < m_ledState.size()) ? m_ledState.at(ledNum).behavior : Behavior::Off;
}

void HSDLeds::clear()
{
    std::fill(m_ledState.begin(), m_ledState.end(), LedState{});
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
    uint32_t index = 0;

    for(const auto& led : m_ledState)
    {
        const bool on =
            led.behavior == Behavior::On ||
            (led.behavior == Behavior::Blinking && m_blinkOn) ||
            (led.behavior == Behavior::Flashing && m_flashOn) ||
            (led.behavior == Behavior::Flickering && m_flickerOn);

        m_stripe->setPixelColor(
            index++,
            on ? static_cast<uint32_t>(led.color) : static_cast<uint32_t>(Color::None));
    }

    m_stripe->show();
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

    const uint32_t third = m_ledState.size() / 3;
    auto setRange = [&](auto first, auto last, Color color)
    {
        for(auto it = first; it != last; ++it)
        {
            it->behavior = Behavior::On;
            it->color = color;
        }
    };

    switch(type)
    {
    case 1: // first row green
        setRange(m_ledState.begin(), m_ledState.begin() + third, Color::Green);
        break;
    case 2: // second row green
        setRange(m_ledState.begin() + third, m_ledState.begin() + 2 * third, Color::Green);
        break;
    case 3: // third row green
        setRange(m_ledState.begin() + 2 * third, m_ledState.end(), Color::Green);
        break;
    case 4: // all green
        setRange(m_ledState.begin(), m_ledState.end(), Color::Green);
        break;
    case 5: // color sweep
    {
        const Color colors[] =
            {
                Color::Red,
                Color::Green,
                Color::Blue};

        for(uint32_t led = 0; led < third; ++led)
        {
            for(Color c : colors)
            {
                for(uint32_t col = 0; col < 3; ++col)
                {
                    uint32_t idx = led + col * third;
                    m_ledState[idx].behavior = Behavior::On;
                    m_ledState[idx].color = c;
                }

                updateStripe();
                delay(50);
            }

            for(uint32_t col = 0; col < 3; ++col)
            {
                uint32_t idx = led + col * third;
                m_ledState[idx].behavior = Behavior::Off;
                m_ledState[idx].color = Color::None;
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
