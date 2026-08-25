#pragma once
#include <stdint.h>

struct joypad {
  uint8_t P1 : 5;
};

void joypad_init(struct joypad *jp);

void joypad_write(struct joypad *jp, uint8_t val);
