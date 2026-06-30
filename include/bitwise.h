#pragma once
#include <stdbool.h>
#include <stdint.h>

static void set_bit(uint8_t *target, uint8_t i, bool val) {
  if (val) {
    *target |= (1 << i);
  } else {
    *target &= ~(1 << i);
  }
}

static bool get_bit(uint8_t target, uint8_t i) { return (target >> i) & 1; }
