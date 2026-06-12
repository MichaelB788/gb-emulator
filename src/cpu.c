#include "cpu.h"
#include <stdint.h>

void init_cpu(CPU *cpu) {
  cpu->r8[0] = 0xFF;
  cpu->r8[1] = 0x13;
  cpu->r8[2] = 0x00;
  cpu->r8[3] = 0xC1;
  cpu->r8[4] = 0x84;
  cpu->r8[5] = 0x03;
  cpu->r8[6] = 0x00;
  cpu->r8[7] = 0x01;
  cpu->PC = 0x0100;
  cpu->SP = 0xFFFE;
}

void set_r16(CPU *cpu, Reg16_Idx r16_idx, uint16_t val) {
  cpu->r8[r16_idx] = val >> 8;
  cpu->r8[r16_idx + 1] = val & 0xFF;
}

uint16_t get_r16(const CPU *cpu, Reg16_Idx r16_idx) {
  return (uint16_t)cpu->r8[r16_idx] << 8 | cpu->r8[r16_idx + 1];
}

void set_z(uint8_t *flags, bool val) {
  if (val) {
    *flags |= 0x80;
  } else {
    *flags &= ~0x80;
  }
}

void set_n(uint8_t *flags, bool val) {
  if (val) {
    *flags |= 0x40;
  } else {
    *flags &= ~0x40;
  }
}

void set_h(uint8_t *flags, bool val) {
  if (val) {
    *flags |= 0x20;
  } else {
    *flags &= ~0x20;
  }
}

void set_c(uint8_t *flags, bool val) {
  if (val) {
    *flags |= 0x10;
  } else {
    *flags &= ~0x10;
  }
}

bool is_z_set(uint8_t flags) { return flags & 0x80; }

bool is_n_set(uint8_t flags) { return flags & 0x40; }

bool is_h_set(uint8_t flags) { return flags & 0x20; }

bool is_c_set(uint8_t flags) { return flags & 0x10; }

bool check_condition(uint8_t flags, Condition cond) {
  switch (cond) {
  case COND_NZ:
    return !is_z_set(flags);
  case COND_Z: // Z
    return is_z_set(flags);
  case COND_NC: // NC
    return !is_c_set(flags);
  case COND_C: // C
    return is_c_set(flags);
  }
}
