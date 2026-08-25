#include "serial.h"
#include <stdint.h>
#include <stdio.h>

void serial_write_sc(struct serial *serial, uint8_t val) {
  static constexpr uint8_t SC_WRITABLE_MASK = 0x83;
  serial->SC = val & SC_WRITABLE_MASK;

  // Print output to console, used in blargg's tests
  if (val == 0x81) {
    putchar(serial->SB);
    fflush(stdout);
  }
}
