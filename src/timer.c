#include "timer.h"
#include "interrupt.h"
#include <stdint.h>

void timer_tick(struct timer *timer, struct interrupt *interrupt) {
  timer->SYS += 4;

  if (timer->TAC & TAC_ENABLE) {
    static const unsigned FREQS[4] = {256 * 4, 4 * 4, 16 * 4, 64 * 4};
    const unsigned freq = FREQS[timer->TAC & TAC_CLOCK_SELECT];

    if ((timer->accumulated_cycles += 4) >= FREQS[freq]) {
      // Increment TIMA, execute the following subroutine upon overflow
      if (++timer->TIMA == 0) {
        timer->TIMA = timer->TMA;
        interrupt->IF |= INTERRUPT_TIMER;
      }

      timer->accumulated_cycles -= freq;
    }
  }
}
