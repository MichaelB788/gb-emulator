#pragma once
#include <stdint.h>

enum interrupt_flags {
  // clang-format off
  INTERRUPT_VBLANK = 1 << 0,
  INTERRUPT_LCD    = 1 << 1,
  INTERRUPT_TIMER  = 1 << 2,
  INTERRUPT_SERIAL = 1 << 3,
  INTERRUPT_JOYPAD = 1 << 4
  // clang-format on
};

struct interrupt {
  uint8_t IF; // 0xFF0F: interrupt flag
  uint8_t IE; // 0xFFFF: interrupt enable
};
