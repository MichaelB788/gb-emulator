#pragma once
#include <stdint.h>

enum joypad_fields {
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
  JOYP_ENABLE_BUTTONS = 1 << 5,

  JOYP_MODES = JOYP_ENABLE_DPAD | JOYP_ENABLE_BUTTONS
  // clang-format on
};

struct joypad {
  uint8_t p1; // 0xFF00: JOYP / P1
};

uint8_t joypad_read(const struct joypad *joypad);
void joypad_write(struct joypad *joypad, uint8_t u8);
