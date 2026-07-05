#pragma once
#include "interrupts.h"
#include <stdint.h>

struct timer {
  uint32_t elapsed_cycles;
  uint16_t system_counter;

  uint8_t div;
  uint8_t counter;
  uint8_t modulo;
  uint8_t control;
};

void timer_tick(struct timer *timer, int t_cycles,
                struct interrupts *interrupt);
