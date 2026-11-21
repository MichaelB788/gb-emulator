#pragma once
#include <stdint.h>

#define JOYP_BUTTON_A 0x01u
#define JOYP_BUTTON_B 0x02u
#define JOYP_BUTTON_SELECT 0x04u
#define JOYP_BUTTON_START 0x08u

#define JOYP_DPAD_RIGHT 0x01u
#define JOYP_DPAD_LEFT 0x02u
#define JOYP_DPAD_UP 0x04u
#define JOYP_DPAD_DOWN 0x08u

#define JOYP_ENABLE_DPAD 0x10u
#define JOYP_ENABLE_BUTTONS 0x20u

struct joypad {
  uint8_t p1; // 0xFF00: JOYP / P1
};

uint8_t joypad_read(const struct joypad *joypad);
void joypad_write(struct joypad *joypad, uint8_t u8);
