#pragma once
#include <stdint.h>

struct interrupt;

struct timer {
  unsigned elapsed_cycles; // Tracks elapsed cycles for timer counting
  uint16_t system_counter; // SYS, full register of 0xFF04: DIV
  uint8_t counter;         // 0xFF05: TIMA
  uint8_t modulo;          // 0xFF06: TMA
  uint8_t control;         // 0xFF07: TAC
};

void timer_init(struct timer *timer);

void timer_tick(struct timer *timer, struct interrupt *interrupt);
