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

void ppu_draw_tile(const struct ppu *ppu, SDL_Color *framebuffer, int x, int y,
                   int pitch) {}

void ppu_draw_vram_tile_data(const struct ppu *ppu, SDL_Color *framebuffer) {
  size_t vram_idx = 0;
  for (int tile_y = 0; tile_y < VRAM_DATA_SCREEN_PIXEL_H / 8; ++tile_y) {
    for (int tile_x = 0; tile_x < VRAM_DATA_SCREEN_PIXEL_W / 8; ++tile_x) {
      for (int pixel_row = 0; pixel_row < 8; ++pixel_row) {
        const size_t vram_offset = vram_idx + (pixel_row * 2);
        for (int pixel_col = 0; pixel_col < 8; ++pixel_col) {
          const uint8_t shft = 7 - pixel_col;

          const bool lsb = ppu->vram[vram_offset + 0] >> shft & 1;
          const bool msb = ppu->vram[vram_offset + 1] >> shft & 1;
          const uint8_t color_idx = msb << 1 | lsb;

          const size_t fb_y = pixel_row + tile_y * 8;
          const size_t fb_x = pixel_col + tile_x * 8;
          framebuffer[fb_y * VRAM_DATA_SCREEN_PIXEL_W + fb_x] =
              LCD_COLOR_PALETTE[color_idx];
        }
      }
      vram_idx += BYTES_PER_TILE;
    }
  }
}
