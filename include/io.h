#pragma once
#include <stdint.h>

typedef struct {
  uint8_t joypad;
  uint8_t serial_data;
  uint8_t serial_control;
} IO;

uint8_t read_io(IO *io, uint16_t addr);

void write_io(IO *io, uint16_t addr, uint8_t val);
