#pragma once

#include "HSDConfig.hpp"
#include <Adafruit_NeoPixel.h>
#include <memory>

class HSDLeds
{
  public:
    explicit HSDLeds(const HSDConfig& config);

    void begin();
    void update();
    void clear();

    void set(uint32_t ledNum, HSDConfig::Behavior behavior, HSDConfig::Color color);
    void setAll(HSDConfig::Behavior behavior, HSDConfig::Color color);

    HSDConfig::Color getColor(uint32_t ledNum) const;
    HSDConfig::Behavior getBehavior(uint32_t ledNum) const;

    void test(uint32_t type);

  private:
    struct LedState
    {
        HSDConfig::Behavior behavior{HSDConfig::OFF};
        HSDConfig::Color color{HSDConfig::NONE};
    };

    // timing constants
    static constexpr uint32_t BLINK_ON_TIME = 500;
    static constexpr uint32_t BLINK_OFF_TIME = 500;
    static constexpr uint32_t FLASH_ON_TIME = 2000;
    static constexpr uint32_t FLASH_OFF_TIME = 200;
    static constexpr uint32_t FLICKER_ON_TIME = 100;
    static constexpr uint32_t FLICKER_OFF_TIME = 100;

    void handleBlink(unsigned long now);
    void handleFlash(unsigned long now);
    void handleFlicker(unsigned long now);

    void updateStripe();

    const HSDConfig& m_config;

    uint32_t m_numLeds{0};
    Adafruit_NeoPixel m_stripe;
    std::unique_ptr<LedState[]> m_ledState;

    bool m_blinkOn{false};
    bool m_flashOn{false};
    bool m_flickerOn{false};

    unsigned long m_prevBlink{0};
    unsigned long m_prevFlash{0};
    unsigned long m_prevFlicker{0};
};
