#pragma once
#include <stdint.h>

typedef struct {
  uint8_t reg;
} joypad;

struct serial_transfer {
  uint8_t data;
  uint8_t control;
};

struct timer {
  uint8_t counter;
  uint8_t modulo;
  uint8_t control;
};

struct interrupt {
  uint8_t flag;
  uint8_t enable;
};

struct hardware_io {
  joypad joypad;
  struct serial_transfer serial;
  struct timer timer;
  struct interrupt interrupt;
};

uint8_t read_io_reg(struct hardware_io *io, uint16_t addr);

void write_io_reg(struct hardware_io *io, uint16_t addr, uint8_t val);
