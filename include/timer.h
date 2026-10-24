#pragma once
#include <stdbool.h>
#include <stdint.h>

struct interrupt;

enum tac_flags { TAC_CLOCK_SELECT = 0x3, TAC_TIMER_ENABLE = 1 << 2 };

struct timer {
  bool interrupt_pending; // timer interrupt is delayed by one m-cycle

  uint16_t system_counter; // SYS, full register of 0xFF04: DIV
  uint8_t counter;         // 0xFF05: TIMA
  uint8_t modulo;          // 0xFF06: TMA
  uint8_t control;         // 0xFF07: TAC
};

void timer_tick(struct timer *timer, struct interrupt *interrupt);
