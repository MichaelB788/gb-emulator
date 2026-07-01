#include "hardware_io.h"
#include <stdint.h>
#include <stdio.h>

enum {
  JOYP_IO = 0xFF00,
  SB_IO = 0xFF01,
  SC_IO = 0xFF02,
  IF_IO = 0xFF0F,
  IE_IO = 0xFFFF,
};

uint8_t read_io_reg(struct hardware_io *io, uint16_t addr) {
  switch (addr) {
  case JOYP_IO:
    return io->joypad.reg;
  case SB_IO:
    return io->serial.data;
  case SC_IO:
    return io->serial.control;
  case IF_IO:
    return io->interrupt.flag;
  case IE_IO:
    return io->interrupt.enable;
  default:
    return 0xFF;
  }
}

void write_io_reg(struct hardware_io *io, uint16_t addr, uint8_t val) {
  switch (addr) {
  case JOYP_IO:
    io->joypad.reg = val & 0x30;
    break;
  case SB_IO:
    io->serial.data = val;
    break;
  case SC_IO:
    if ((val & 0x80) > 0) {
      putchar(io->serial.data);
      fflush(stdout);
    }
    break;
  case IF_IO:
    io->interrupt.flag = val & 0x1F;
    break;
  case IE_IO:
    io->interrupt.enable = val & 0x1F;
    break;
  default:
    break;
  }
}
