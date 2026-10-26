#pragma once
#include <stdint.h>

struct serial_transfer {
  uint8_t sb; // 0xFF01: SB
  uint8_t sc; // 0xFF02: SC
};

void serial_transfer_write_control(struct serial_transfer *serial, uint8_t u8);
