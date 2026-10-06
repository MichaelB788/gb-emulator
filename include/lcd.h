#pragma once
#include <stdint.h>

enum lcd_control_flags {
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

enum lcd_status_flags {
  // clang-format off
  STAT_PPU_MODE          = 0x3,    // R
  STAT_LYC_EQ_LY         = 1 << 2, // R
  STAT_MODE_0_INT_SELECT = 1 << 3, // R/W
  STAT_MODE_1_INT_SELECT = 1 << 4, // R/W
  STAT_MODE_2_INT_SELECT = 1 << 5, // R/W
  STAT_LYC_INT_SELECT    = 1 << 6  // R/W
  // clang-format on
};

struct lcd {
  uint8_t control;      // 0xFF40: LCDC
  uint8_t status;       // 0xFF41: STAT
  uint8_t y_coordinate; // 0xFF44: LY
  uint8_t compare;      // 0xFF45: LYC
};
