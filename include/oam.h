#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct bus;

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
  uint8_t dma; // 0xFF46: OAM DMA source address

  bool dma_transfer_in_progress; // writing to DMA starts a transfer
  uint16_t dma_src_a16, dma_dest_a16;

  struct oam_entry entries[40];
};

void oam_tick(struct oam *oam, const struct bus *bus);

void oam_write_dma_u8(struct oam *oam, uint8_t u8);

uint8_t oam_read_u8(const struct oam *oam, uint16_t a16);
void oam_write_u8(struct oam *oam, uint16_t a16, uint8_t u8);
