#pragma once
#include <stdint.h>

#define TAC_CLOCK_SELECT 0x3
#define TAC_ENABLE (1 << 2)
#define TAC_UNUSED 0xF8

struct timer {
  unsigned elapsed_cycles; // Tracks cycles elapsed, used with TIMA
  uint16_t system_counter; // Internal system counter

  uint8_t DIV;  // Timer divider, upper byte of the system counter
  uint8_t TIMA; // Timer counter
  uint8_t TMA;  // Timer modulo
  uint8_t TAC;  // Timer control
};

struct interrupts;

void timer_tick(struct timer *timer, struct interrupts *interrupts);
