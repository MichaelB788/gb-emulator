#include "serial.h"
#include <assert.h>
#include <stdio.h>

uint8_t serial_read(const struct serial_transfer *serial, enum serial_reg reg) {
  if (reg == SERIAL_DATA_REG) {
    return serial->data;
  } else if (reg == SERIAL_CONTROL_REG) {
    return serial->control;
  }
  assert(0 && "impossible serial read");
}

void serial_write(struct serial_transfer *serial, enum serial_reg reg,
                  uint8_t val) {
  if (reg == SERIAL_DATA_REG) {
    serial->data = val;
  } else if (reg == SERIAL_CONTROL_REG) {
    serial->control = val;
    if (val & 0x80) {
      putchar(serial->data);
      fflush(stdout);
    }
  }
}
