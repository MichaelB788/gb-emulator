#include "timer.h"
#include "interrupt.h"
#include <stdint.h>

void timer_tick(struct timer *timer, struct interrupt *interrupt) {
  if (timer->interrupt_pending) {
    timer->interrupt_pending = false;

    if (timer->TIMA == 0) {
      timer->TIMA = timer->TMA;
      interrupt->IF |= INTERRUPT_TIMER;
    }
    // if TIMA was already overriden, nothing happens
  }

  const uint16_t sys_before = timer->SYS;
  timer->SYS += 4;

  if (timer->TAC & TAC_ENABLE) {
    const uint8_t watched_bit = 1 + ((timer->TAC & TAC_CLOCK_SELECT) * 2);
    const uint16_t mask = 1 << watched_bit;

    // detect a falling edge on the watched_bit of SYS
    if ((sys_before & mask) != 0 && (timer->SYS & mask) == 0)
      timer->interrupt_pending = ++timer->TIMA == 0;
  }
}
