#pragma once
#include <stdbool.h>
#include <stdint.h>

static void set_bit(uint8_t *target, uint8_t i) { *target |= 1 << i; }

static void clear_bit(uint8_t *target, uint8_t i) { *target &= ~(1 << i); }

static void toggle_bit(uint8_t *target, uint8_t i) { *target ^= 1 << i; }

static bool is_bit_set(uint8_t target, uint8_t i) { return target >> i & 1; }

static void set_bit_as(uint8_t *target, uint8_t i, bool val) {
  if (val) {
    *target |= 1 << i;
  } else {
    *target &= ~(1 << i);
  }
}
