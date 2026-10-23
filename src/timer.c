#include "timer.h"
#include "interrupt.h"
#include <stdint.h>

void timer_tick(struct timer *timer, struct interrupt *interrupt) {
  timer->system_counter += 4;

  if (timer->control & TAC_TIMER_ENABLE) {
    static const unsigned FREQS[4] = {256 * 4, 4 * 4, 16 * 4, 64 * 4};
    const unsigned freq = FREQS[timer->control & TAC_CLOCK_SELECT];

    if ((timer->accumulated_cycles += 4) >= FREQS[freq]) {
      // Increment TIMA, execute the following subroutine upon overflow
      if (++timer->counter == 0) {
        timer->counter = timer->modulo;
        interrupt->flag |= INTERRUPT_TIMER;
      }

      timer->accumulated_cycles -= freq;
    }
  }
}
