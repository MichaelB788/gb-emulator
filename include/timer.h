#pragma once
#include <stdbool.h>
#include <stdint.h>

struct interrupt;

enum tac_fields { TAC_CLOCK_SELECT = 0x3, TAC_ENABLE = 1 << 2 };

struct timer {
  bool interrupt_pending; // timer interrupt is delayed by one m-cycle

  // internal system counter, the upper byte is exposed to the bus at 0xFF04:
  // timer divider (DIV)
  uint16_t sys;

  uint8_t tima; // 0xFF05: timer counter
  uint8_t tma;  // 0xFF06: timer modulo
  uint8_t tac;  // 0xFF07: timer control
};

void timer_tick(struct timer *timer, struct interrupt *interrupt);
