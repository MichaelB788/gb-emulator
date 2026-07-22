#pragma once
#include <stdbool.h>
#include <stdint.h>

struct interrupts {
  uint8_t flag;
  uint8_t enable;
};
