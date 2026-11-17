#include "lcd.h"
#include <SDL3/SDL_pixels.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

uint16_t lcd_lcdc_bg_tilemap_area(const struct lcd *lcd) {
  // note: going from 0x9800 to 0x9C00 is a matter of setting the 10th bit
  const bool lcdc_bit3 = (lcd->lcdc & LCDC_BG_TILE_MAP_AREA) != 0;
  return 0x9800 | lcdc_bit3 << 10;
}

uint16_t lcd_lcdc_bg_win_addressing_mode(const struct lcd *lcd) {
  // note: going from 0x9000 to 0x8000 is a matter of flipping the 12th bit
  const bool lcdc_bit4 = (lcd->lcdc & LCDC_BG_WIN_ADDRESSING_MODE) != 0;
  return 0x9000 ^ lcdc_bit4 << 12;
}

uint16_t lcd_lcdc_win_tilemap_area(const struct lcd *lcd) {
  // note: going from 0x9800 to 0x9C00 is a matter of setting the 10th bit
  const bool lcdc_bit6 = (lcd->lcdc & LCDC_WIN_TILE_MAP_AREA) != 0;
  return 0x9800 | lcdc_bit6 << 10;
}

void lcd_stat_write(struct lcd *lcd, uint8_t u8) {
  // Lower 3-bits read only, 7th bit is unused
  lcd->stat = lcd->stat & 0x7 | u8 & 0x78;
}

void lcd_stat_update_ppu_mode(struct lcd *lcd, enum ppu_mode mode) {
  lcd->stat = lcd->stat & ~STAT_PPU_MODE | mode;
}

void lcd_stat_update_lyc_equals_ly(struct lcd *lcd) {
  lcd->stat = lcd->stat & ~STAT_LYC_EQUALS_LY | (lcd->ly == lcd->lyc) << 2;
}

enum ppu_mode lcd_stat_ppu_mode(const struct lcd *lcd) {
  return lcd->lcdc & LCDC_LCD_ENABLED ? lcd->stat & STAT_PPU_MODE : PPU_HBLANK;
}
