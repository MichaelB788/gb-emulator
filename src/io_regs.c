#include "io_regs.h"
#include <stdint.h>
#include <stdio.h>

uint8_t read_io_reg(struct io_regs *io, uint16_t addr) {
  switch (addr) {
  case JOYPAD_IO:
    return io->joypad;

  case SERIAL_TRANSFER_DATA_IO:
    return io->serial_transfer_data;

  case INTERRUPT_FLAG_IO:
    return io->interrupt_flag;

  case INTERRUPT_ENABLE_IO:
    return io->interrupt_enable;

  case LCD_Y_IO:
    return 0x90;

  default:
    return 0xFF;
  }
}

void write_io_reg(struct io_regs *io, uint16_t addr, uint8_t val) {
  switch (addr) {
  case JOYPAD_IO:
    io->joypad = val & 0x30;
    break;

  case SERIAL_TRANSFER_DATA_IO:
    io->serial_transfer_data = val;
    break;

  case SERIAL_TRANSFER_CONTROL_IO:
    if ((val & 0x80) > 0) {
      putchar(io->serial_transfer_data);
      fflush(stdout);
    }
    break;

  case INTERRUPT_FLAG_IO:
    io->interrupt_flag = val & 0x1F;
    break;

  case INTERRUPT_ENABLE_IO:
    io->interrupt_enable = val & 0x1F;
    break;

  default:
    break;
  }
}
