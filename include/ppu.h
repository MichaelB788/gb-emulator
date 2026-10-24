#pragma once
#include <stdint.h>

struct interrupt;

enum lcdc_flags {
  // clang-format off
  LCDC_BG_AND_WINDOW_ENABLE = 1 << 0,
  LCDC_OBJ_ENABLE           = 1 << 1,
  LCDC_OBJ_SIZE             = 1 << 2,
  LCDC_BG_TILE_MAP          = 1 << 3,
  LCDC_BG_AND_WINDOW_TILES  = 1 << 4,
  LCDC_WINDOW_ENABLE        = 1 << 5,
  LCDC_WINDOW_TILE_MAP      = 1 << 6,
  LCDC_LCD_AND_PPU_ENABLE   = 1 << 7
  // clang-format on
};

enum stat_flags {
  // clang-format off
  STAT_PPU_MODE          = 0x3,    // R
  STAT_LYC_EQ_LY         = 1 << 2, // R
  STAT_MODE_0_INT_SELECT = 1 << 3, // R/W
  STAT_MODE_1_INT_SELECT = 1 << 4, // R/W
  STAT_MODE_2_INT_SELECT = 1 << 5, // R/W
  STAT_LYC_INT_SELECT    = 1 << 6  // R/W
  // clang-format on
};

enum ppu_mode {
  PPU_HORIZONTAL_BLANK = 0,
  PPU_VERTICAL_BLANK = 1,
  PPU_OAM_SCAN = 2,
  PPU_DRAWING_PIXELS = 3
};

struct ppu {
  enum ppu_mode mode;
  unsigned accumulated_dots;

  uint8_t LCDC; // 0xFF40: LCD control
  uint8_t STAT; // 0xFF41: LCD status
  uint8_t SCY;  // 0xFF42: scroll Y (viewport Y position)
  uint8_t SCX;  // 0xFF43: scroll X (viewport X position)
  uint8_t LY;   // 0xFF44: line Y (current scanline)
  uint8_t LYC;  // 0xFF45: LY compare
  uint8_t BGP;  // 0xFF47: background palette data

  // memory maps
  uint8_t vram[8 * 1024]; // [0x8000, 0x9FFF]
  uint8_t oam[160];       // [0xFE00, 0xFE9F]

  uint32_t framebuffer[160 * 144];
};

void ppu_tick(struct ppu *ppu, struct interrupt *interrupt);

uint8_t ppu_read_vram(const struct ppu *ppu, uint16_t a16);
uint8_t ppu_read_oam(const struct ppu *ppu, uint16_t a16);

void ppu_write_vram(struct ppu *ppu, uint16_t a16, uint8_t u8);
void ppu_write_oam(struct ppu *ppu, uint16_t a16, uint8_t u8);
