#pragma once
#include <SDL3/SDL_pixels.h>
#include <stdbool.h>
#include <stdint.h>

#define SCREEN_PIXL_W 160ul
#define SCREEN_PIXL_H 144ul

static const SDL_Color LCD_COLOR_PALETTE[4] = {
    [0] = {.r = 0xb9, .g = 0xe0, .b = 0x8d, .a = 0xff}, // Light green
    [1] = {.r = 0x8b, .g = 0xa8, .b = 0x6a, .a = 0xff}, // Green
    [2] = {.r = 0x69, .g = 0x7f, .b = 0x50, .a = 0xff}, // Dark green
    [3] = {.r = 0x3d, .g = 0x49, .b = 0x2e, .a = 0xff}  // Darker green
};

enum lcdc_fields {
  /**
   * if this bit is set to 0:
   * - background and window become blank. window display
   * - only objects are displayed if LCDC_OBJ_ENABLED is set
   * - LCDC_WIN_ENABLE is ignored
   */
  LCDC_BG_WIN_ENABLED = 1 << 0,

  // toggles whether objects are displayed
  LCDC_OBJ_ENABLED = 1 << 1,

  // changes tile size (1 tile or 2 stacked vertically)
  LCDC_OBJ_SIZE = 1 << 2,

  // if clear, BG uses tilemap 0x9800, otherwise 0x9C00
  LCDC_BG_TILE_MAP_AREA = 1 << 3,

  /**
   * addressing mode for BG & WIN to pick tiles from
   * - tiles are always indexed by an 8-bit (un)signed integer
   * - 1: 0x8000 is the base pointer & unsigned addressing is used
   * - 0: 0x9000 is the base pointer & signed addressing is used
   */
  LCDC_BG_WIN_ADDRESSING_MODE = 1 << 4,

  // controls whether the window is displayed or not. overridden by
  // LCDC_BG_WIN_ENABLED if that is cleared
  LCDC_WIN_ENABLED = 1 << 5,

  // if clear, WIN uses tilemap 0x9800, otherwise 0x9C00
  LCDC_WIN_TILE_MAP_AREA = 1 << 6,

  // if clear, turn off the LCD & PPU, grant immediate access to VRAM & OAM.
  LCDC_LCD_ENABLED = 1 << 7
};

enum stat_fields {
  // clang-format off
  STAT_PPU_MODE          = 0x3,    // read only
  STAT_LYC_EQUALS_LY     = 1 << 2, // read only

  STAT_MODE_0_INT_SELECT = 1 << 3, // read/write
  STAT_MODE_1_INT_SELECT = 1 << 4, // read/write
  STAT_MODE_2_INT_SELECT = 1 << 5, // read/write
  STAT_LYC_INT_SELECT    = 1 << 6  // read/write
  // clang-format on
};

// reported by STAT.ppu_mode (defaults to 0 if ppu disabled)
enum ppu_mode { PPU_HBLANK = 0, PPU_VBLANK, PPU_OAM_SCAN, PPU_DRAWING };

struct lcd {
  uint8_t lcdc; // 0xFF40: LCD control
  uint8_t stat; // 0xFF41: LCD status
  uint8_t scy;  // 0xFF42: scroll Y (viewport Y position)
  uint8_t scx;  // 0xFF43: scroll X (viewport X position)
  uint8_t ly;   // 0xFF44: current scanline
  uint8_t lyc;  // 0xFF45: LY compare
  uint8_t bgp;  // 0xFF47: Background (and window) palette
  uint8_t obp0; // 0xFF48: OBJ palette data 0
  uint8_t obp1; // 0xFF49: OBJ palette data 1
};

uint16_t lcd_lcdc_win_tilemap_area(const struct lcd *lcd);
uint16_t lcd_lcdc_bg_win_addressing_mode(const struct lcd *lcd);

void lcd_write_stat(struct lcd *lcd, uint8_t u8);
enum ppu_mode lcd_stat_ppu_mode(const struct lcd *lcd);

SDL_Color lcd_get_bg_pixel(const struct lcd *lcd, uint8_t color_idx);
SDL_Color lcd_get_obj_pixel(const struct lcd *lcd, uint8_t color_idx,
                            uint8_t obp);
