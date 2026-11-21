#include "ppu.h"
#include "SDL3/SDL_rect.h"
#include "lcd.h"
#include "oam.h"
#include "pixel_display.h"
#include <SDL3/SDL_render.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void ppu_tick(struct ppu *ppu, struct bus *bus) {
  // todo
  // oam_tick(&ppu->oam, bus);
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
                   uint8_t palette, SDL_Point point,
                   struct pixel_display *display) {
  // a16 must be within the tile data section
  assert(0x8000 <= tile_data_a16 && tile_data_a16 <= 0x97FF);

  // a16 must be a multiple of 16
  assert((tile_data_a16 & 0xF) == 0);

  static const SDL_Color GB_PALETTE[4] = {
      [0] = {.r = 0xb9, .g = 0xe0, .b = 0x8d, .a = 0xff}, // Light green
      [1] = {.r = 0x8b, .g = 0xa8, .b = 0x6a, .a = 0xff}, // Green
      [2] = {.r = 0x69, .g = 0x7f, .b = 0x50, .a = 0xff}, // Dark green
      [3] = {.r = 0x3d, .g = 0x49, .b = 0x2e, .a = 0xff}  // Darker green
  };

  const size_t tile_data_entry_idx = tile_data_a16 & 0x1FF0;
  const SDL_Rect screen_dimensions = {.w = display->screen->w,
                                      .h = display->screen->h};

  for (uint8_t row = 0; row < 8; ++row) {
    const size_t vram_idx = tile_data_entry_idx + (row * 2);
    for (uint8_t col = 0; col < 8; ++col) {
      const SDL_Point pixel_point = {point.x + col, point.y + row};
      if (SDL_PointInRect(&pixel_point, &screen_dimensions)) {
        const uint8_t shft = 7 - col;
        const bool lsb = ppu->vram[vram_idx + 0] >> shft & 1;
        const bool msb = ppu->vram[vram_idx + 1] >> shft & 1;
        const size_t color_idx = (palette >> ((msb << 1 | lsb) * 2)) & 0x3;

        pixel_display_draw_pixel(display, pixel_point, GB_PALETTE[color_idx]);
      }
    }
  }
}

void ppu_draw_vram_tile_data(const struct ppu *ppu,
                             struct pixel_display *display) {
  static const uint16_t BYTES_PER_TILE = 16u;
  static const uint8_t DEFAULT_PALETTE = 0xE4; // 0b11100100

  for (uint16_t a16 = 0x8000; a16 < 0x9800; a16 += BYTES_PER_TILE) {
    const uint8_t tile_id = a16 >> 4 & 0xFF;
    const SDL_Point tile_point = {
        .x = (tile_id & 0xF) * 8,
        .y = ((tile_id >> 4 & 0xF) + (a16 >= 0x9000 ? 16 : 0)) * 8};

    ppu_draw_tile(ppu, a16, DEFAULT_PALETTE, tile_point, display);
  }
}
