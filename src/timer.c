#include "timer.h"
#include "bitwise.h"
#include "interrupts.h"

#define CPU_CLOCK 4194304u

void timer_tick(struct timer *timer, int t_cycles,
                struct interrupts *interrupt) {
  timer->elapsed_cycles += t_cycles;
  timer->system_counter += t_cycles;
  timer->divider = timer->system_counter >> 8; // DIV is just the visible part

  // Check to see if timer is enabled
  if (is_bit_set(timer->control, 2)) {
    static const unsigned timer_period[] = {
        CPU_CLOCK / 4096u, CPU_CLOCK / 262144u, CPU_CLOCK / 65536u,
        CPU_CLOCK / 16384u};
    int selected_freq = timer_period[timer->control & 0b11];
    if (timer->elapsed_cycles >= selected_freq) {
      // Increment the timer counter and check for overflow
      if (++timer->counter == 0x00) {
        timer->counter = timer->modulo;
        set_bit(&interrupt->flag, 2); // Request a timer interrupt
      }
      timer->elapsed_cycles = timer->elapsed_cycles % selected_freq;
    }
  }
}
