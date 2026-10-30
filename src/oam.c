#include "oam.h"
#include "bus.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void oam_tick(struct oam *oam, const struct bus *bus) {
  if (oam->dma_transfer_in_progress) {
    for (int n = 0; n < 4; ++n) {
      const uint16_t src = oam->dma_src_a16++;
      const uint16_t dest = oam->dma_dest_a16++;
      oam_write_u8(oam, dest, bus_read(bus, src));
    }

    oam->dma_transfer_in_progress = oam->dma_dest_a16 < 0xFE9F;
  }
}

void oam_write_dma_u8(struct oam *oam, uint8_t u8) {
  oam->dma = u8;
  oam->dma_transfer_in_progress = true;
  oam->dma_src_a16 = (uint16_t)u8 << 8;
  oam->dma_dest_a16 = 0xFE00;
}

uint8_t oam_read_u8(const struct oam *oam, uint16_t a16) {
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

void oam_write_u8(struct oam *oam, uint16_t a16, uint8_t u8) {
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
