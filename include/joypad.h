#pragma once
#include <stdint.h>

#define JOYP_A (1 << 0)
#define JOYP_B (1 << 1)
#define JOYP_SELECT (1 << 2)
#define JOYP_START (1 << 3)

#define JOYP_RIGHT (1 << 0)
#define JOYP_LEFT (1 << 1)
#define JOYP_UP (1 << 2)
#define JOYP_DOWN (1 << 3)

#define JOYP_DPAD_SELECT (1 << 4)
#define JOYP_BUTTONS_SELECT (1 << 5)
#define JOYP_MODE_SELECT 0x30

#define JOYP_UNUSED 0xC0

struct joypad {
  uint8_t JOYP;
};
