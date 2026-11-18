#pragma once
#include <stdint.h>

#define INTERRUPT_VBLANK 0x01u
#define INTERRUPT_LCD 0x02u
#define INTERRUPT_TIMER 0x04u
#define INTERRUPT_SERIAL 0x08u
#define INTERRUPT_JOYPAD 0x10u

struct interrupt {
  uint8_t flag;   // 0xFF0F: IF
  uint8_t enable; // 0xFFFF: IE
};
