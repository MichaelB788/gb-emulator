#pragma once
#include <stdbool.h>
#include <stdint.h>

struct interrupts {
  uint8_t flag;
  uint8_t enable;
};

static bool interrupt_pending(const struct interrupts *interrupt) {
  return (interrupt->enable & interrupt->flag) > 0;
}

struct gameboy;

int service_interrupts(struct gameboy *gb);
