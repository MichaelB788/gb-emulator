#include "ppu.h"
#include "lcd.h"
#include "oam.h"
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

static void ppu_draw_tile(const struct ppu *ppu, SDL_Color *framebuffer,
                          int tile_x, int tile_y, int bytes_read) {
  for (int pixel_y = 0; pixel_y < 8; ++pixel_y) {
    const size_t vram_idx = bytes_read + (pixel_y * 2);
    for (int pixel_x = 0; pixel_x < 8; ++pixel_x) {
      const uint8_t shft = 7 - pixel_x;
      const bool lsb = ppu->vram[vram_idx + 0] >> shft & 1;
      const bool msb = ppu->vram[vram_idx + 1] >> shft & 1;

      const size_t fb_y = pixel_y + tile_y * 8;
      const size_t fb_x = pixel_x + tile_x * 8;
      framebuffer[fb_y * VRAM_DATA_SCREEN_PIXEL_W + fb_x] =
          LCD_COLOR_PALETTE[msb << 1 | lsb];
    }
  }
}

void ppu_draw_vram_tile_data(const struct ppu *ppu, SDL_Color *framebuffer) {
  size_t bytes_read = 0;
  for (int tile_y = 0; tile_y < VRAM_DATA_SCREEN_PIXEL_H / 8; ++tile_y) {
    for (int tile_x = 0; tile_x < VRAM_DATA_SCREEN_PIXEL_W / 8; ++tile_x) {
      ppu_draw_tile(ppu, framebuffer, tile_x, tile_y, bytes_read);
      bytes_read += BYTES_PER_TILE;
    }
  }
}
