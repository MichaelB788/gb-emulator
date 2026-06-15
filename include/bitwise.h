#pragma once
#include <stdbool.h>
#include <stdint.h>

static inline void set_bit(uint8_t *byte, uint8_t b3_idx, bool value) {
  if (value)
    *byte |= 1 << b3_idx;
  else
    *byte &= ~(1 << b3_idx);
}

static inline bool get_bit(uint8_t byte, uint8_t b3_idx) {
  return (byte >> b3_idx) & 1;
}
