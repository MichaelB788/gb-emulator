#include "io_registers.h"
#include <stdint.h>
#include <stdio.h>

uint8_t read_io(struct io_registers *io, uint16_t addr) {
  switch (addr) {
  case 0xFF00: // Joypad
    return io->joypad;
  case 0xFF01: // Serial transfer data
    return io->serial_data;
  case 0xFF02: // Serial transfer control
    return io->serial_control;
  default:
    return 0xFF;
  }
}

void write_io(struct io_registers *io, uint16_t addr, uint8_t val) {
  switch (addr) {
  case 0xFF00: // Joypad
    io->joypad = val & 0x30;
    break;
  case 0xFF01: // Serial transfer data
    io->serial_data = val;
    break;
  case 0xFF02: // Serial transfer control
    io->serial_control = val;
    if ((val & 0x80) > 0) {
      putchar(io->serial_data);
      fflush(stdout);
    }
    break;
  }
}
