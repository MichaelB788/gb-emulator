#pragma once
#include <stdint.h>

enum joypad_flags {
  // clang-format off
  JOYP_BUTTON_A      = 1 << 0,
  JOYP_BUTTON_B      = 1 << 1,
  JOYP_BUTTON_SELECT = 1 << 2,
  JOYP_BUTTON_START  = 1 << 3,

  JOYP_DPAD_RIGHT = 1 << 0,
  JOYP_DPAD_LEFT  = 1 << 1,
  JOYP_DPAD_UP    = 1 << 2,
  JOYP_DPAD_DOWN  = 1 << 3,

  JOYP_ENABLE_DPAD    = 1 << 4,
  JOYP_ENABLE_BUTTONS = 1 << 5
  // clang-format on
};

struct joypad {
  uint8_t P1; // 0xFF00: joypad inputs
};
