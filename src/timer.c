#include "timer.h"
#include "interrupt.h"
#include <stdint.h>

void timer_tick(struct timer *timer, struct interrupt *interrupt) {
  if (timer->interrupt_pending) {
    timer->interrupt_pending = false;

    if (timer->counter == 0) {
      timer->counter = timer->modulo;
      interrupt->flag |= INTERRUPT_TIMER;
    }
    // if TIMA was already overriden, nothing happens
  }

  const uint16_t sys_prev = timer->system_counter;
  timer->system_counter += 4;

  if (timer->control & TAC_TIMER_ENABLE) {
    const uint8_t watched_bit = 1 + ((timer->control & TAC_CLOCK_SELECT) * 2);
    const uint16_t mask = 1 << watched_bit;

    // detect a falling edge on the watched_bit of SYS
    if ((sys_prev & mask) != 0 && (timer->system_counter & mask) == 0)
      timer->interrupt_pending = ++timer->counter == 0;
  }
}
