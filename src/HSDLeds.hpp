#pragma once

#include "HSDConfig.hpp"
#include <Adafruit_NeoPixel.h>
#include <memory>

class HSDLeds
{
  public:
    explicit HSDLeds(const HSDConfig& config) noexcept;

    void begin();
    void update() noexcept;
    void clear() noexcept;

    void set(uint32_t ledNum, Behavior behavior, Color color) noexcept;
    void setAll(Behavior behavior, Color color) noexcept;

    [[nodiscard]] Color getColor(uint32_t ledNum) const noexcept;
    [[nodiscard]] Behavior getBehavior(uint32_t ledNum) const noexcept;

    void test(uint32_t type);

  private:
    struct LedState
    {
        Behavior behavior = Behavior::Off;
        Color color = Color::None;
    };

    // timing constants (ms)
    static constexpr uint32_t BLINK_ON_TIME = 500;
    static constexpr uint32_t BLINK_OFF_TIME = 500;
    static constexpr uint32_t FLASH_ON_TIME = 2000;
    static constexpr uint32_t FLASH_OFF_TIME = 200;
    static constexpr uint32_t FLICKER_ON_TIME = 100;
    static constexpr uint32_t FLICKER_OFF_TIME = 100;

    void updateBlink(uint32_t now) noexcept;
    void updateFlash(uint32_t now) noexcept;
    void updateFlicker(uint32_t now) noexcept;

    void updateStripe() noexcept;

    [[nodiscard]] bool isLedOn(const LedState& led) const noexcept;

    const HSDConfig& m_config;

    std::unique_ptr<Adafruit_NeoPixel> m_stripe;
    std::vector<LedState> m_ledState;

    bool m_blinkOn = false;
    bool m_flashOn = false;
    bool m_flickerOn = false;

    uint32_t m_prevBlink = 0;
    uint32_t m_prevFlash = 0;
    uint32_t m_prevFlicker = 0;
};
