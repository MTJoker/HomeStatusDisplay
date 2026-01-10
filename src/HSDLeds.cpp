#include "HSDLeds.hpp"
#include <algorithm>

HSDLeds::HSDLeds(const HSDConfig& config) noexcept
    : m_config(config)
{
}

void HSDLeds::begin()
{
    const auto numLeds = m_config.getNumberOfLeds();
    m_ledState.assign(numLeds, LedState{});

    const auto pin = m_config.getLedDataPin();
    const auto type = (m_config.getLedType() == 0) ? NEO_GRB : NEO_GRBW;

    m_stripe = std::make_unique<Adafruit_NeoPixel>(numLeds, pin, type);

    m_stripe->setBrightness(m_config.getLedBrightness());
    m_stripe->begin();

    clear();
    updateStripe();
}

void HSDLeds::set(uint32_t ledNum, Behavior behavior, Color color) noexcept
{
    if(ledNum >= m_ledState.size())
    {
        return;
    }

    m_ledState[ledNum] = {behavior, color};
}

void HSDLeds::setAll(Behavior behavior, Color color) noexcept
{
    std::fill(m_ledState.begin(), m_ledState.end(), LedState{behavior, color});
}

Color HSDLeds::getColor(uint32_t ledNum) const noexcept
{
    return (ledNum < m_ledState.size()) ? m_ledState[ledNum].color : Color::None;
}

Behavior HSDLeds::getBehavior(uint32_t ledNum) const noexcept
{
    return (ledNum < m_ledState.size()) ? m_ledState[ledNum].behavior : Behavior::Off;
}

void HSDLeds::clear() noexcept
{
    std::fill(m_ledState.begin(), m_ledState.end(), LedState{});
}

void HSDLeds::update() noexcept
{
    const uint32_t now = millis();

    updateBlink(now);
    updateFlash(now);
    updateFlicker(now);

    updateStripe();
}

bool HSDLeds::isLedOn(const LedState& led) const noexcept
{
    switch(led.behavior)
    {
    case Behavior::On:
        return true;
    case Behavior::Blinking:
        return m_blinkOn;
    case Behavior::Flashing:
        return m_flashOn;
    case Behavior::Flickering:
        return m_flickerOn;
    default:
        return false;
    }
}

void HSDLeds::updateStripe() noexcept
{
    uint32_t index = 0;

    for(const auto& led : m_ledState)
    {
        const uint32_t color =
            isLedOn(led) ? static_cast<uint32_t>(led.color)
                         : static_cast<uint32_t>(Color::None);

        m_stripe->setPixelColor(index++, color);
    }

    m_stripe->show();
}

void HSDLeds::updateBlink(uint32_t now) noexcept
{
    const uint32_t interval = m_blinkOn ? BLINK_OFF_TIME : BLINK_ON_TIME;
    if((now - m_prevBlink) >= interval)
    {
        m_blinkOn = !m_blinkOn;
        m_prevBlink = now;
    }
}

void HSDLeds::updateFlash(uint32_t now) noexcept
{
    const uint32_t interval = m_flashOn ? FLASH_OFF_TIME : FLASH_ON_TIME;
    if((now - m_prevFlash) >= interval)
    {
        m_flashOn = !m_flashOn;
        m_prevFlash = now;
    }
}

void HSDLeds::updateFlicker(uint32_t now) noexcept
{
    const uint32_t interval = m_flickerOn ? FLICKER_OFF_TIME : FLICKER_ON_TIME;
    if((now - m_prevFlicker) >= interval)
    {
        m_flickerOn = !m_flickerOn;
        m_prevFlicker = now;
    }
}

void HSDLeds::test(uint32_t type)
{
    clear();

    const uint32_t third = m_ledState.size() / 3;

    auto setRange = [&](uint32_t start, uint32_t end, Color color)
    {
        for(uint32_t i = start; i < end; ++i)
        {
            m_ledState[i] = {Behavior::On, color};
        }
    };

    switch(type)
    {
    case 1: // first row green
        setRange(0, third, Color::Green);
        break;

    case 2: // second row green
        setRange(third, 2 * third, Color::Green);
        break;

    case 3: // third row green
        setRange(2 * third, m_ledState.size(), Color::Green);
        break;

    case 4: // all green
        setRange(0, m_ledState.size(), Color::Green);
        break;

    case 5: // color sweep (blocking!)
    {
        static constexpr Color colors[] = {
            Color::Red,
            Color::Green,
            Color::Blue};

        for(uint32_t led = 0; led < third; ++led)
        {
            for(Color c : colors)
            {
                for(uint32_t col = 0; col < 3; ++col)
                {
                    const uint32_t idx = led + col * third;
                    m_ledState[idx] = {Behavior::On, c};
                }

                updateStripe();
                delay(50);
            }

            for(uint32_t col = 0; col < 3; ++col)
            {
                const uint32_t idx = led + col * third;
                m_ledState[idx] = {Behavior::Off, Color::None};
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

    updateStripe();
}
