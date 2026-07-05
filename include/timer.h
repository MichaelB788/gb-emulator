#pragma once
#include <stdint.h>

struct timer {
  unsigned elapsed_cycles;
  uint16_t system_counter;

  uint8_t divider;
  uint8_t counter;
  uint8_t modulo;
  uint8_t control;
};

struct interrupts;

void timer_tick(struct timer *timer, int t_cycles,
                struct interrupts *interrupt);
