#include "ppu.h"
#include "lcd.h"
#include "oam.h"
#include "pixel_display.h"
#include <SDL3/SDL_render.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void ppu_tick(struct ppu *ppu, struct bus *bus) {
  // TODO: lcd tick
  oam_tick(&ppu->oam, bus);
}

static bool ppu_is_vram_accessible(const struct ppu *ppu) {
  return lcd_stat_ppu_mode(&ppu->lcd) != PPU_DRAWING ||
         (ppu->lcd.lcdc & LCDC_LCD_ENABLED) == 0;
}

static bool ppu_is_oam_accessible(const struct ppu *ppu) {
  const enum ppu_mode mode = lcd_stat_ppu_mode(&ppu->lcd);
  return (mode != PPU_OAM_SCAN && mode != PPU_DRAWING) ||
         (ppu->lcd.lcdc & LCDC_LCD_ENABLED) == 0;
}

uint8_t ppu_read_vram(const struct ppu *ppu, uint16_t a16) {
  return ppu_is_vram_accessible(ppu) ? ppu->vram[a16 - 0x8000] : 0xFF;
}

void ppu_write_vram(struct ppu *ppu, uint16_t a16, uint8_t u8) {
  if (ppu_is_vram_accessible(ppu))
    ppu->vram[a16 - 0x8000] = u8;
}

uint8_t ppu_read_oam(const struct ppu *ppu, uint16_t a16) {
  return ppu_is_oam_accessible(ppu) ? oam_read_u8(&ppu->oam, a16) : 0xFF;
}

void ppu_write_oam(struct ppu *ppu, uint16_t a16, uint8_t u8) {
  if (ppu_is_oam_accessible(ppu))
    oam_write_u8(&ppu->oam, a16, u8);
}

void ppu_draw_tile(const struct ppu *ppu, uint16_t tile_data_a16,
                   uint8_t palette, int x, int y,
                   struct pixel_display *display) {
  // coord must be within bounds
  assert(x >= 0 && y >= 0 && x <= (display->screen->w - 8) &&
         y <= (display->screen->h - 8));

  // a16 must be a multiple of 16
  assert((tile_data_a16 & 0xF) == 0);

  // NOTE: 384 tile entries can live in the VRAM tile data section
  // 1000000000000000 0x8000
  // 1000100000000000 0x8800
  // 1001000000000000 0x9000
  // 1001011111111111 0x97FF
  // 0001111111110000 0x1FF0
  // 000101111111 383
  const size_t vram_offset = tile_data_a16 & 0x1FF0;

  for (uint8_t pixel_y = 0; pixel_y < 8; ++pixel_y) {
    const size_t vram_idx = vram_offset + (pixel_y * 2);
    for (uint8_t pixel_x = 0; pixel_x < 8; ++pixel_x) {
      const uint8_t shft = 7 - pixel_x;
      const bool lsb = ppu->vram[vram_idx + 0] >> shft & 1;
      const bool msb = ppu->vram[vram_idx + 1] >> shft & 1;
      const size_t palette_idx = palette >> ((msb << 1 | lsb) * 2) & 0x3;

      const size_t fb_x = x + pixel_x;
      const size_t fb_y = y + pixel_y;
      display->framebuffer[fb_y * display->screen->w + fb_x] =
          LCD_COLOR_PALETTE[palette_idx];
    }
  }
}

void ppu_draw_vram_tile_data(const struct ppu *ppu,
                             struct pixel_display *display) {
  static const uint8_t DEFAULT_PALETTE = 0xE4; // 0b11100100
  for (uint16_t a16 = 0x8000; a16 < 0x9800; a16 += BYTES_PER_TILE) {
    const uint8_t tile_id = a16 >> 4 & 0xFF;
    const int tile_x = (tile_id & 0xF);
    const int tile_y = ((tile_id >> 4 & 0xF) + (a16 >= 0x9000 ? 16 : 0));

    ppu_draw_tile(ppu, a16, DEFAULT_PALETTE, tile_x * 8, tile_y * 8, display);
  }
}
