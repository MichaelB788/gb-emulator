#pragma once
#include <stdbool.h>
#include <stdint.h>

struct interrupt {
  uint8_t flag;
  uint8_t enable;
};

static uint8_t interrupt_get_pending(const struct interrupt *interrupt) {
  return interrupt->enable & interrupt->flag;
}
