#include "lcd.h"
#include <SDL3/SDL_pixels.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

uint16_t lcd_lcdc_win_tilemap_area(const struct lcd *lcd) {
  // note: going from 0x9800 to 0x9C00 is a matter of setting the 10th bit
  return 0x9800 | (lcd->lcdc & LCDC_WIN_TILE_MAP_AREA) << 4;
}

uint16_t lcd_lcdc_bg_win_addressing_mode(const struct lcd *lcd) {
  // note: going from 0x8000 to 0x9000 is a matter of setting the 12th bit
  return 0x8000 | (lcd->lcdc & LCDC_BG_WIN_ADDRESSING_MODE) << 8;
}

void lcd_write_stat(struct lcd *lcd, uint8_t u8) {
  // Lower 3-bits read only, 7th bit is unused
  lcd->stat = lcd->stat & 0x7 | u8 & 0x78;
}

enum ppu_mode lcd_stat_ppu_mode(const struct lcd *lcd) {
  return lcd->lcdc & LCDC_LCD_ENABLED ? lcd->stat & STAT_PPU_MODE : 0;
}

// color_idx = (hi >> col & 1) << 1 | (lo >> col & 1)
SDL_Color lcd_get_bg_pixel(const struct lcd *lcd, uint8_t color_idx) {
  assert(color_idx <= 3);
  return LCD_COLOR_PALETTE[lcd->bgp >> (color_idx * 2) & 0x3];
}

SDL_Color lcd_get_obj_pixel(const struct lcd *lcd, uint8_t color_idx,
                            uint8_t obp) {
  assert(color_idx <= 3 && (obp == lcd->obp0 || obp == lcd->obp1));
  // note: color index 0 is transparent
  return color_idx != 0 ? LCD_COLOR_PALETTE[obp >> (color_idx * 2) & 0x3]
                        : (SDL_Color){};
}
