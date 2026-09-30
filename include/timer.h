#pragma once
#include <stdint.h>

struct interrupt;

struct timer {
  unsigned elapsed_cycles; // Tracks elapsed cycles for timer counting
  uint16_t system_counter; // SYS
  uint8_t counter;         // TIMA
  uint8_t modulo;          // TMA
  uint8_t control;         // TAC
};

void timer_init(struct timer *timer);

void timer_tick(struct timer *timer, struct interrupt *interrupt);
