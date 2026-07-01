#include "interrupt.h"
#include <assert.h>
#include <stdint.h>

uint8_t interrupt_read(const struct interrupts *interrupt,
                       enum interrupt_reg reg) {
  if (reg == INTERRUPT_FLAG_REG) {
    return interrupt->flag;
  } else if (reg == INTERRUPT_ENABLE_REG) {
    return interrupt->enable;
  }
  assert(0 && "impossible interrupt read");
}

void interrupt_write(struct interrupts *interrupt, enum interrupt_reg reg,
                     uint8_t val) {
  if (reg == INTERRUPT_FLAG_REG) {
    interrupt->flag = val & 0x1F;
  } else if (reg == INTERRUPT_ENABLE_REG) {
    interrupt->enable = val & 0x1F;
  }
}
