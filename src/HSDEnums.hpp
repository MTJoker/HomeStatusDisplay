#pragma once

#include <cstdint>

/*
 * Enum which defines the types of devices which can send messages.
 * If the same message (e.g. "on") can be received from different types
 * of devices (e.g. light and alarm), different reaction can be done.
 */
enum class DeviceType : uint8_t
{
    Window,
    Door,
    Light,
    Alarm,
    Unknown
};

enum class Behavior : uint8_t
{
    Off,
    On,
    Blinking,
    Flashing,
    Flickering
};

enum class Color : uint32_t
{
    None = 0x000000,
    Green = 0x00FF00,
    Yellow = 0xFFFF00,
    Orange = 0xFF5500,
    Red = 0xFF0000,
    Purple = 0xFF00FF,
    Blue = 0x0000FF,
    White = 0xFFFFFF
};
