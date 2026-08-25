#pragma once
#include <stdint.h>

struct serial {
  uint8_t SB; // Serial transfer data
  uint8_t SC; // Serial transfer control
};

void serial_write_sc(struct serial *serial, uint8_t val);
