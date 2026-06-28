#pragma once
#include <stdint.h>

enum io_reg_addr {
  JOYPAD_IO = 0xFF00,
  SERIAL_TRANSFER_DATA_IO = 0xFF01,
  SERIAL_TRANSFER_CONTROL_IO = 0xFF02,
  INTERRUPT_FLAG_IO = 0xFF0F,
  INTERRUPT_ENABLE_IO = 0xFFFF,
  LCD_Y_IO = 0xFF44,
};

struct io_regs {
  uint8_t joypad;
  uint8_t serial_transfer_data;
  uint8_t interrupt_flag;
  uint8_t interrupt_enable;
};

uint8_t read_io_reg(struct io_regs *io, uint16_t addr);

void write_io_reg(struct io_regs *io, uint16_t addr, uint8_t val);
