#pragma once
#include <stdint.h>

inline void set_bit(uint8_t *byte, uint8_t bit_index) {
  *byte |= 1 << bit_index;
}

inline void clear_bit(uint8_t *byte, uint8_t bit_index) {
  *byte &= ~(1 << bit_index);
}

inline uint8_t get_bit(uint8_t byte, uint8_t bit_index) {
  return (byte >> bit_index) & 1;
}
