#pragma once
#include <stdint.h>

struct serial_transfer {
  uint8_t SB; // 0xFF01: serial transfer data
  uint8_t SC; // 0xFF02: serial transfer control
};

void serial_transfer_write_control(struct serial_transfer *serial, uint8_t u8);
