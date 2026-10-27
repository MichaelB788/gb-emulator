#pragma once
#include <stdint.h>

enum oam_entry_flags {
  // clang-format off
  OBJ_DMG_PALETTE = 1 << 4,
  OBJ_X_FLIP      = 1 << 5,
  OBJ_Y_FLIP      = 1 << 6,
  OBJ_PRIORITY    = 1 << 7
  // clang-format on
};

struct oam_entry {
  uint8_t y;          // byte 0
  uint8_t x;          // byte 1
  uint8_t tile_index; // byte 2
  uint8_t flags;      // byte 3
};

struct oam {
  uint8_t dma; // 0xFF46: OAM DMA source address & start
  struct oam_entry entries[40];
};

void oam_tick(struct oam *oam);
