#include "timer.h"
#include "interrupt.h"
#include <stdint.h>

void timer_tick(struct timer *timer, struct interrupt *interrupt) {
  if (timer->interrupt_pending) {
    timer->interrupt_pending = false;

    if (timer->tima == 0) {
      timer->tima = timer->tma;
      interrupt->flag |= INTERRUPT_TIMER;
    }
    // if TIMA was already overriden, nothing happens
  }

  const uint16_t sys_prev = timer->sys;
  timer->sys += 4;

  if (timer->tac & TAC_ENABLE) {
    static const uint16_t watched_bit[4] = {
        // increment TIMA every ...
        [0x1] = 1 << 3, // 4 M-cycles
        [0x2] = 1 << 5, // 16 M-cycles
        [0x3] = 1 << 7, // 64 M-cycles
        [0x0] = 1 << 9  // 256 M-cycles
    };
    const uint16_t mask = watched_bit[timer->tac & TAC_CLOCK_SELECT];

    // falling edge detector
    if ((sys_prev & mask) != 0 && (timer->sys & mask) == 0) {
      if (++timer->tima == 0)
        timer->interrupt_pending = true;
    }
  }
}
