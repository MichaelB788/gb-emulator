#pragma once
#include <stdbool.h>
#include <stdint.h>

static inline void set_bit(uint8_t *byte, uint8_t bit_index, bool value) {
  if (value)
    *byte |= (uint8_t)(1 << bit_index);
  else
    *byte &= (uint8_t)~(1 << bit_index);
}

static inline bool get_bit(uint8_t byte, uint8_t bit_index) {
  return (byte >> bit_index) & 1;
}
