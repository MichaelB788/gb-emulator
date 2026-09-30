#include "serial_transfer.h"
#include <stdint.h>
#include <stdio.h>

void serial_transfer_write_control(struct serial_transfer *serial, uint8_t u8) {
  if ((serial->control = u8 & 0x81) == 0x81) {
    putchar(serial->data);
    fflush(stdout);
  }
}
