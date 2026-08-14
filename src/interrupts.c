#include "interrupts.h"
#include "bus.h"
#include "cpu.h"
#include <stdint.h>

uint8_t interrupts_pending(struct interrupts interrupts) {
  return interrupts.IE & interrupts.IF;
}

// See: https://gbdev.io/pandocs/Interrupts.html#interrupt-handling
void interrupts_service_pending(struct interrupts *interrupts,
                                struct cpu *cpu) {
  const uint8_t pending = interrupts_pending(*interrupts);
  if (pending != 0) {
    cpu->state = CPU_RUNNING;
  } else {
    return;
  }

  if (cpu->IME) {
    for (uint8_t i = 0; i < 5; ++i) {
      if ((pending & (1 << i)) != 0) {
        // Two wait states are executed
        bus_tick(cpu->bus);
        bus_tick(cpu->bus);

        // Call the interrupt handler at it's address around 0x40
        cpu_call_a16(cpu, (uint16_t)(0x40 | (i << 3)), true);

        // Interrupt handled
        interrupts->IF &= ~(1 << i);
        cpu->IME = false;
        return;
      }
    }
  }
}
