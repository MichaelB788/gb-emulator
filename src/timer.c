#include "timer.h"
#include "constants.h"
#include "interrupts.h"
#include <stddef.h>
#include <stdint.h>

void timer_write_div(struct timer *timer, uint8_t val) {
  // Writing to DIV resets the system counter
  timer->system_counter = 0;
  timer->DIV = 0;
}

static constexpr uint8_t TAC_ENABLE = 1 << 2;
static constexpr uint8_t TAC_CLOCK_SELECT_MASK = 0x3;

static constexpr size_t FREQS[] = {[0b00] = CPU_CLOCK_HZ / 4096ul,
                                   [0b01] = CPU_CLOCK_HZ / 262144ul,
                                   [0b10] = CPU_CLOCK_HZ / 65536ul,
                                   [0b11] = CPU_CLOCK_HZ / 16384ul};

void timer_tick(struct timer *timer, struct interrupts *interrupts) {
  timer->system_counter += 4;
  timer->DIV = timer->system_counter >> 8;

  if ((timer->TAC & TAC_ENABLE) != 0) {
    timer->elapsed_cycles += 4;
    const size_t selected = timer->TAC & TAC_CLOCK_SELECT_MASK;
    if (timer->elapsed_cycles >= FREQS[selected]) {
      if (++timer->TIMA == 0) {
        timer->TIMA = timer->TMA;
        interrupts->IF |= INTERRUPT_TIMER;
      }

      timer->elapsed_cycles -= FREQS[selected];
    }
  }
}
