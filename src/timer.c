#include "timer.h"
#include "bitwise.h"

void timer_tick(struct timer *timer, int t_cycles,
                struct interrupts *interrupt) {
  timer->elapsed_cycles += t_cycles;

  timer->system_counter += t_cycles;
  timer->div = timer->system_counter >> 8; // DIV is just the visible part

  if (is_bit_set(timer->control, 2)) /* Timer enabled */ {
    static const int freqs[] = {4096, 262144, 65536, 16384};
    int selected_freq = freqs[timer->control & 0b11];
    if (timer->elapsed_cycles >= selected_freq) {
      timer->elapsed_cycles = timer->elapsed_cycles % selected_freq;

      // Increment and check for overflow
      if (++timer->counter == 0x00) {
        set_bit(&interrupt->flag, 2); // Request a timer interrupt
        timer->counter = timer->modulo;
      }
    }
  }
}
