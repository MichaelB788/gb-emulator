#include "oam.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void oam_tick(struct oam *oam) {
  // TODO: OAM DMA transfer
}

uint8_t oam_read(const struct oam *oam, uint16_t a16) {
  const size_t i = a16 >> 2 & 0x3F;
  // clang-format off
  switch (a16 & 0x3) {
  case 0: return oam->entries[i].y;
  case 1: return oam->entries[i].x;
  case 2: return oam->entries[i].tile_index;
  case 3: return oam->entries[i].flags;
  default: assert(false);
  }
  // clang-format on
}

void oam_write(struct oam *oam, uint16_t a16, uint8_t u8) {
  const size_t i = a16 >> 2 & 0x3F;
  // clang-format off
  switch (a16 & 0x3) {
  case 0: oam->entries[i].y = u8; break;
  case 1: oam->entries[i].x = u8; break;
  case 2: oam->entries[i].tile_index = u8; break;
  case 3: oam->entries[i].flags = u8; break;
  default: assert(false);
  }
  // clang-format on
}
