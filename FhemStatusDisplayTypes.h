#pragma once

#include "Led.h"

// Helper macro for getting the number of elements in an array
#define NUMBER_OF_ELEMENTS(array)  (sizeof(array)/sizeof(array[0]))

#define MAX_COLOR_MAPPING_MSG_LEN 20

/*
 * Enum which defines the types of devices which can send messages.
 * If the same message (e.g. "on") can be received from different types
 * of devices (e.g. light and alarm), different reaction can be done.
 */
enum deviceType
{
  TYPE_WINDOW,
  TYPE_DOOR,
  TYPE_LIGHT,
  TYPE_ALARM
};

/*
 * This struct is used for mapping a device of a specific device type 
 * to a led number, that means a specific position on the led stripe
 */
struct deviceMapping
{
  String name;      // name of the device
  deviceType type;  // type of the device
  int ledNumber;    // led number on which reactions for this device are displayed
};

/*
 * This struct is used for mapping a message for a specific device
 * type to a led behavior (see LedSwitcher::ledState).
 */
struct colorMapping
{
  char msg[MAX_COLOR_MAPPING_MSG_LEN+1];            // message 
  deviceType type;         // type of the device
  Led::Color color;        // led color for message from device type
  Led::Behavior behavior;  // led behavior for message from device type
};

