#pragma once
#include <stdint.h>

struct interrupt;

enum tac_flags { TAC_CLOCK_SELECT = 0x3, TAC_ENABLE = 1 << 2 };

struct timer {
  unsigned accumulated_cycles; // Tracks elapsed T-cycles when TAC[2] is enabled

  uint16_t SYS; // internal system clock, full register of 0xFF04: divider (DIV)

  uint8_t TIMA; // 0xFF05: timer accumulator
  uint8_t TMA;  // 0xFF06: timer modulo
  uint8_t TAC;  // 0xFF07: timer control
};

void timer_tick(struct timer *timer, struct interrupt *interrupt);
