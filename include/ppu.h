#pragma once
#include <stdint.h>

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

struct ppu {
  struct {
    uint8_t viewport_y_pos; // 0xFF42: SCY
    uint8_t viewport_x_pos; // 0xFF43: SCX
  } background;

  struct {
    uint8_t control;      // 0xFF40: LCDC
    uint8_t status;       // 0xFF41: STAT
    uint8_t y_coordinate; // 0xFF44: LY
    uint8_t compare;      // 0xFF45: LYC
  } lcd;

  uint8_t vram[8 * 1024]; // 8 KiB video ram, [0x8000, 0x9FFF]
  uint8_t oam[160];       // Object Attribute Memory, [0xFE00, 0xFE9F]
};

uint8_t ppu_read_vram(const struct ppu *ppu, uint16_t a16);
uint8_t ppu_read_oam(const struct ppu *ppu, uint16_t a16);

void ppu_write_vram(struct ppu *ppu, uint16_t a16, uint8_t u8);
void ppu_write_oam(struct ppu *ppu, uint16_t a16, uint8_t u8);
