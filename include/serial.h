#pragma once
#include "interrupt.h"
#include <stdint.h>

enum serial_reg { SERIAL_DATA_REG = 0xFF01, SERIAL_CONTROL_REG = 0xFF02 };

struct serial_transfer {
  uint8_t data;
  uint8_t control;
};

uint8_t serial_read(const struct serial_transfer *serial, enum serial_reg reg);

void serial_write(struct serial_transfer *serial, struct interrupts *interrupt,
                  enum serial_reg reg, uint8_t val);
