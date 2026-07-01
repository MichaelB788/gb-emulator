#pragma once
#include <stdint.h>

enum interrupt_reg {
  INTERRUPT_FLAG_REG = 0xFF0F,
  INTERRUPT_ENABLE_REG = 0xFFFF
};

struct interrupts {
  uint8_t flag;
  uint8_t enable;
};

uint8_t interrupt_read(const struct interrupts *interrupt,
                       enum interrupt_reg reg);

void interrupt_write(struct interrupts *interrupt, enum interrupt_reg reg,
                     uint8_t val);
