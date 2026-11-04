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

static void ppu_draw_tile(const struct ppu *ppu, size_t offset) {
  for (size_t y = 0; y < 8; ++y) {
    for (size_t x = 0; x < 8; ++x) {
    }
  }
}

void ppu_draw_vram_tile_data(const struct ppu *ppu, SDL_Color *framebuffer) {
  for (int y = 0; y < VRAM_TILE_DATA_TILE_H; ++y) {
    for (int x = 0; x < VRAM_TILE_DATA_TILE_W; ++x) {
      const int i = x * 2;
      const uint8_t lo = ppu->vram[i];
      const uint8_t hi = ppu->vram[i + 1];
      for (int px = 0; px < 8; ++px) {
        const uint8_t shift = 7 - px;
        const uint8_t color_idx = (hi >> shift & 1) << 1 | (lo >> shift & 1);

        framebuffer[px + x + (y * VRAM_TILE_DATA_PIXEL_W)] =
            lcd_get_color_bg_win(&ppu->lcd, color_idx);
      }
    }
  }
}
