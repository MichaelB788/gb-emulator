#include "interrupts.h"
#include "bus.h"
#include "cpu.h"
#include <stdint.h>

// See: https://gbdev.io/pandocs/Interrupts.html#interrupt-handling
void interrupts_service_pending(struct interrupts *in, struct cpu *cpu) {
  const uint8_t pending = in->IE & in->IF;
  for (uint8_t i = 0; i < 5; ++i) {
    if ((pending & 1 << i) != 0) {
      // Two wait states are executed
      bus_tick(cpu->bus);
      bus_tick(cpu->bus);

      // Call the interrupt handler at it's address around 0x40
      cpu_call(cpu, (uint16_t)(0x40 | (i << 3)), true);

      in->IF &= ~(1 << i);
      cpu->IME = false;
      return; // Interrupt handled
    }
  }
}
