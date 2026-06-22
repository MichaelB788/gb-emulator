#pragma once
#include <stdint.h>

struct io_registers {
  uint8_t joypad;
  uint8_t serial_data;
};

uint8_t read_io(struct io_registers *io, uint16_t addr);

void write_io(struct io_registers *io, uint16_t addr, uint8_t val);
