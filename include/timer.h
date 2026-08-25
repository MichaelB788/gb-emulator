#pragma once
#include <stddef.h>
#include <stdint.h>

struct timer {
  size_t elapsed_cycles;   // Tracks cycles elapsed, used with TIMA
  uint16_t system_counter; // Hidden internal system counter

  uint8_t DIV;     // Timer divider, upper byte of `system_counter`
  uint8_t TIMA;    // Timer counter
  uint8_t TMA;     // Timer modulo
  uint8_t TAC : 3; // Timer control
};

struct interrupts;

void timer_write_div(struct timer *timer, uint8_t val);

void timer_tick(struct timer *timer, struct interrupts *interrupts);
