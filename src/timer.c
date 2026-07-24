#include "timer.h"
#include "bitwise.h"
#include <stdint.h>

#define CPU_CLOCK_HZ 4194304u

void timer_tick(struct timer *timer, uint8_t *interrupt_flag) {
  timer->elapsed_cycles += 4;
  timer->system_counter += 4;
  timer->divider = timer->system_counter >> 8; // DIV is just the visible part

  // Check to see if timer is enabled
  if (is_bit_set(timer->control, 2)) {
    // T-cycles taken until TIMA is incremented
    static const unsigned max_cycles[] = {[0b00] = CPU_CLOCK_HZ / 4096u,
                                          [0b01] = CPU_CLOCK_HZ / 262144u,
                                          [0b10] = CPU_CLOCK_HZ / 65536u,
                                          [0b11] = CPU_CLOCK_HZ / 16384u};
    const uint8_t clock_select = timer->control & 0b11;
    if (timer->elapsed_cycles >= max_cycles[clock_select]) {
      if (++timer->counter == 0x00) {
        timer->counter = timer->modulo;
        set_bit(interrupt_flag, 2); // Request a timer interrupt
      }

      timer->elapsed_cycles = timer->elapsed_cycles % max_cycles[clock_select];
    }
  }
}
