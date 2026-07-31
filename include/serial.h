#pragma once
#include <stdint.h>

#define SC_CLOCK_SELECT (1 << 0)
#define SC_CLOCK_SPEED (1 << 1)
#define SC_TRANSFER_ENABLE (1 << 7)
#define SC_UNUSED 0x7C

struct serial {
  uint8_t SB; // Serial transfer data
  uint8_t SC; // Serial transfer control
};
