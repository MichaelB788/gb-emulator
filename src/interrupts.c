#include "interrupts.h"
#include "bitwise.h"
#include "gameboy.h"
#include "instructions.h"
#include <stdint.h>

int handle_interrupts(struct gameboy *gb) {
  if (!gb->cpu.IME)
    return 0;

  const uint8_t pending_interrupts = gb->interrupt.flag & gb->interrupt.enable;
  for (uint8_t i = 0; i < 5; ++i) {
    if (is_bit_set(pending_interrupts, i)) {
      push_n16(gb, gb->cpu.PC);
      gb->cpu.PC = 0x40 | (i << 3);

      clear_bit(&gb->interrupt.flag, i);
      gb->cpu.enable_interrupts = false;
      gb->cpu.IME = false;

      return 20;
    }
  }

  return 0;
}
