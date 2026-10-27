#include "joypad.h"
#include <stdint.h>

uint8_t joypad_read(const struct joypad *joypad) {
  // If no mode is enabled, then all button inputs are considered released
  // (in the case of the GameBoy, a released input is 1 due to pull-up
  // resistors)
  return (joypad->p1 & JOYP_MODES) != 0 ? joypad->p1 : 0xFF;
}

void joypad_write(struct joypad *joypad, uint8_t u8) {
  // Lower nibble is read only, unused bits default to 1
  joypad->p1 = joypad->p1 & ~JOYP_MODES | u8 & JOYP_MODES;
}
