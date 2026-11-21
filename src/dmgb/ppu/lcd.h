#pragma once
#include <SDL3/SDL_pixels.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * if this bit is set to 0:
 * - background and window become blank. window display
 * - only objects are displayed if LCDC_OBJ_ENABLED is set
 * - LCDC_WIN_ENABLE is ignored
 */
#define LCDC_BG_WIN_ENABLED 0x01u

// toggles whether objects are displayed
#define LCDC_OBJ_ENABLED 0x02u

// changes tile size (1 tile or 2 stacked vertically)
#define LCDC_OBJ_SIZE 0x04u

// if clear, BG uses tilemap 0x9800, otherwise 0x9C00
#define LCDC_BG_TILE_MAP_AREA 0x08u

/**
 * addressing mode for BG & WIN to pick tiles from
 * - tiles are always indexed by an 8-bit (un)signed integer
 * - 1: 0x8000 is the base pointer & unsigned addressing is used
 * - 0: 0x9000 is the base pointer & signed addressing is used
 */
#define LCDC_BG_WIN_ADDRESSING_MODE 0x10u

/**
 * controls whether the window is displayed or not. overridden by
 * LCDC_BG_WIN_ENABLED if that is cleared
 */
#define LCDC_WIN_ENABLED 0x20u

// if clear, WIN uses tilemap 0x9800, otherwise 0x9C00
#define LCDC_WIN_TILE_MAP_AREA 0x40u

// if clear, turn off the LCD & PPU, grant immediate access to VRAM & OAM.
#define LCDC_LCD_ENABLED 0x80u

// indictates the PPU's current status (see enum ppu_mode)
#define STAT_PPU_MODE 0x03u

// contains the result of LY == LYC. updated constantly
#define STAT_LYC_EQUALS_LY 0x04u

// if set, triggers a STAT interrupt if STAT_PPU_MODE == PPU_HBLANK
#define STAT_HBLANK_SELECT 0x08u

// if set, triggers a STAT interrupt if STAT_PPU_MODE == PPU_VBLANK
#define STAT_VBLANK_SELECT 0x10u

// if set, triggers a STAT interrupt if STAT_PPU_MODE == PPU_OAM_SCAN
#define STAT_OAM_SCAN_SELECT 0x20u

// if set, triggers a STAT interrupt if STAT_LYC_EQUALS_LY
#define STAT_LYC_LY_SELECT 0x40u

// reported by STAT.ppu_mode (defaults to 0 if ppu disabled)
enum ppu_mode {
  // waiting until the end of the scanline, 80 dots
  PPU_HBLANK = 0x0,

  // waiting until the next frame, between 172 & 289 dots
  PPU_VBLANK = 0x1,

  // searching for OBJs which overlap this line, 376 - (vblank duration) dots
  PPU_OAM_SCAN = 0x2,

  // sending pixels to the LCD 4560 dots (10 scanlines)
  PPU_DRAWING = 0x3
};

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

uint16_t lcd_lcdc_bg_tilemap_area(const struct lcd *lcd);
uint16_t lcd_lcdc_bg_win_addressing_mode(const struct lcd *lcd);
uint16_t lcd_lcdc_win_tilemap_area(const struct lcd *lcd);

void lcd_stat_write(struct lcd *lcd, uint8_t u8);
void lcd_stat_update_ppu_mode(struct lcd *lcd, enum ppu_mode mode);
void lcd_stat_update_lyc_equals_ly(struct lcd *lcd);
enum ppu_mode lcd_stat_ppu_mode(const struct lcd *lcd);
