#include "lcd.h"
#include <assert.h>
#include <stdint.h>

void lcd_init(struct lcd *lcd) {
  for (size_t i = 0; i < LCD_SCREEN_PIXL_W * LCD_SCREEN_PIXL_H; ++i)
    lcd->framebuffer[i] = LCD_COLOR_PALETTE[0];
}

void lcd_write_stat(struct lcd *lcd, uint8_t u8) {
  // Lower 3-bits read only, 7th bit is unused
  lcd->stat = lcd->stat & 0x7 | u8 & 0x78;
}

enum ppu_mode lcd_get_ppu_mode(const struct lcd *lcd) {
  return lcd->stat & STAT_PPU_MODE;
}

void lcd_draw_pixel_bg(struct lcd *lcd, uint8_t x, uint8_t y,
                       uint8_t color_idx) {
  // (hi >> col & 1) << 1 | (lo >> col & 1)
  const size_t i = x + y * LCD_SCREEN_PIXL_W;
  switch (color_idx) {
  case 0:
    lcd->framebuffer[i] = LCD_COLOR_PALETTE[lcd->bgp & COLOR_IDX_0];
    break;
  case 1:
    lcd->framebuffer[i] = LCD_COLOR_PALETTE[(lcd->bgp & COLOR_IDX_1) >> 2];
    break;
  case 2:
    lcd->framebuffer[i] = LCD_COLOR_PALETTE[(lcd->bgp & COLOR_IDX_2) >> 4];
    break;
  case 3:
    lcd->framebuffer[i] = LCD_COLOR_PALETTE[(lcd->bgp & COLOR_IDX_3) >> 6];
    break;
  default:
    assert(false);
  }
}

static void lcd_draw_obj_pixel(struct lcd *lcd, uint8_t x, uint8_t y,
                               uint8_t obp, uint8_t color_idx) {
  const size_t i = x + y * LCD_SCREEN_PIXL_W;
  switch (color_idx) {
  case 0:
    break; // color index 0 is transparent for objects
  case 1:
    lcd->framebuffer[i] = LCD_COLOR_PALETTE[(obp & COLOR_IDX_1) >> 2];
    break;
  case 2:
    lcd->framebuffer[i] = LCD_COLOR_PALETTE[(obp & COLOR_IDX_2) >> 4];
    break;
  case 3:
    lcd->framebuffer[i] = LCD_COLOR_PALETTE[(obp & COLOR_IDX_3) >> 6];
    break;
  default:
    assert(false);
  }
}

void lcd_draw_pixel_obj0(struct lcd *lcd, uint8_t x, uint8_t y,
                         uint8_t color_idx) {
  lcd_draw_obj_pixel(lcd, x, y, lcd->obp0, color_idx);
}

void lcd_draw_pixel_obj1(struct lcd *lcd, uint8_t x, uint8_t y,
                         uint8_t color_idx) {
  lcd_draw_obj_pixel(lcd, x, y, lcd->obp1, color_idx);
}
