#pragma once
#include <stdbool.h>
#include <stdint.h>

static void set_bit(uint8_t *target, uint8_t i) { *target |= 1 << i; }
static void clear_bit(uint8_t *target, uint8_t i) { *target &= ~(1 << i); }
static void toggle_bit(uint8_t *target, uint8_t i) { *target ^= 1 << i; }

static bool is_bit_set(uint8_t target, uint8_t i) { return target >> i & 1; }
static bool are_any_bits_set(uint8_t target, uint8_t mask) {
  return (target & mask) != 0;
}
static bool are_all_bits_set(uint8_t target, uint8_t mask) {
  return (target & mask) == mask;
}

static void write_bit(uint8_t *target, uint8_t i, bool val) {
  if (val) {
    *target |= 1 << i;
  } else {
    *target &= ~(1 << i);
  }
}
