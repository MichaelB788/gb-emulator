#include "timer.h"
#include "interrupt.h"
#include <stdint.h>

void timer_tick(struct timer *timer, struct interrupt *interrupt) {
  // NOTE: M-cycle to T-cycle translation is just T = M * 4
  static const unsigned FREQS[] = {256 * 4, 4 * 4, 16 * 4, 64 * 4};

  timer->system_counter += 4;
  if (timer->control & 0x4) {
    timer->elapsed_cycles += 4;
    const uint8_t clk_sel = timer->control & 0x3;
    if (timer->elapsed_cycles >= FREQS[clk_sel]) {
      if (++timer->counter == 0) {
        timer->counter = timer->modulo;
        interrupt->flag |= INTERRUPT_TIMER;
      }
      timer->elapsed_cycles -= FREQS[clk_sel];
    }
  }
}
