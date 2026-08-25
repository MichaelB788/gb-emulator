#include "joypad.h"

static constexpr uint8_t JOYP_MODE_SELECT = 0x30;

void joypad_init(struct joypad *jp) {
  jp->P1 = 0x1F; // All inputs are released on initialization
}

void joypad_write(struct joypad *jp, uint8_t val) {
  // only the upper two bits are writable
  const uint8_t written = val & JOYP_MODE_SELECT;
  if (written == JOYP_MODE_SELECT) {
    jp->P1 = 0x1F; // If no mode is selected, all buttons are released
  } else {
    jp->P1 &= ~JOYP_MODE_SELECT;
    jp->P1 |= written;
  }
}
