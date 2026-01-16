#pragma once

#include "HSDEnums.hpp"

inline const __FlashStringHelper* toString(DeviceType t) noexcept
{
    switch(t)
    {
    case DeviceType::Window:
        return F("Window");
    case DeviceType::Door:
        return F("Door");
    case DeviceType::Light:
        return F("Light");
    case DeviceType::Alarm:
        return F("Alarm");
    default:
        return F("Unknown");
    }
}

inline const __FlashStringHelper* toString(Behavior b) noexcept
{
    switch(b)
    {
    case Behavior::Off:
        return F("Off");
    case Behavior::On:
        return F("On");
    case Behavior::Blinking:
        return F("Blinking");
    case Behavior::Flashing:
        return F("Flashing");
    case Behavior::Flickering:
        return F("Flickering");
    default:
        return F("Unknown");
    }
}

inline const __FlashStringHelper* toString(Color b) noexcept
{
    switch(b)
    {
    case Color::None:
        return F("None");
    case Color::Green:
        return F("Green");
    case Color::Yellow:
        return F("Yellow");
    case Color::Orange:
        return F("Orange");
    case Color::Red:
        return F("Red");
    case Color::Purple:
        return F("Purple");
    case Color::Blue:
        return F("Blue");
    case Color::White:
        return F("White");
    default:
        return F("Unknown");
    }
}
