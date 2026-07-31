#include "timer.h"
#include "constants.h"
#include "interrupts.h"
#include <stdint.h>

void timer_tick(struct timer *timer, struct interrupts *interrupts) {
  timer->elapsed_cycles += 4;
  timer->system_counter += 4;
  timer->DIV = timer->system_counter >> 8;

  if ((timer->TAC & TAC_ENABLE) != 0) {
    static const unsigned clocks[] = {[0b00] = CPU_CLOCK_HZ / 4096u,
                                      [0b01] = CPU_CLOCK_HZ / 262144u,
                                      [0b10] = CPU_CLOCK_HZ / 65536u,
                                      [0b11] = CPU_CLOCK_HZ / 16384u};
    const unsigned selected = timer->TAC & TAC_CLOCK_SELECT;
    if (timer->elapsed_cycles >= clocks[selected]) {
      if (++timer->TIMA == 0) {
        timer->TIMA = timer->TMA;
        interrupts->IF |= INTERRUPT_TIMER;
      }

      timer->elapsed_cycles = timer->elapsed_cycles % clocks[selected];
    }
  }
}
