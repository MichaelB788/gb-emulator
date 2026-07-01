#pragma once
#include <stdint.h>

struct timer {
  uint8_t div;
  uint8_t counter;
  uint8_t modulo;
  uint8_t control;
};
