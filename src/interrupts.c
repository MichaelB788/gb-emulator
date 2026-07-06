#include "interrupts.h"
#include "bitwise.h"
#include "gameboy.h"
#include "instructions.h"
#include <stdint.h>

int service_interrupts(struct gameboy *gb) {
  const uint8_t pending_interrupts = gb->interrupt.enable & gb->interrupt.flag;
  for (uint8_t i = 0; i < 5; ++i) {
    if (is_bit_set(pending_interrupts, i)) {
      // Call the interrupt handler source
      push_n16(gb, gb->cpu.PC);
      gb->cpu.PC = 0x40 | (i << 3);

      clear_bit(&gb->interrupt.flag, i);
      gb->cpu.IME = false;
      return 20;
    }
  }
  return 0;
}
