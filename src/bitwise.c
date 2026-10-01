#include "bitwise.h"

void u8_write_mask(uint8_t *u8, uint8_t mask, bool val) {
  if (val)
    *u8 |= mask;
  else
    *u8 &= ~mask;
}
