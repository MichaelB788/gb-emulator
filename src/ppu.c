#include "ppu.h"
#include "interrupt.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static const SDL_Color PPU_COLOR_PALETTE[4] = {
    [0] = {.r = 0xb9, .g = 0xe0, .b = 0x8d, .a = 0xff}, // Light green
    [1] = {.r = 0x8b, .g = 0xa8, .b = 0x6a, .a = 0xff}, // Green
    [2] = {.r = 0x69, .g = 0x7f, .b = 0x50, .a = 0xff}, // Dark green
    [3] = {.r = 0x3d, .g = 0x49, .b = 0x2e, .a = 0xff}  // Darker green
};

void ppu_init(struct ppu *ppu) {
  for (size_t i = 0; i < SCREEN_PIXL_W * SCREEN_PIXL_H; ++i)
    ppu->framebuffer[i] = PPU_COLOR_PALETTE[0];
}

void ppu_tick(struct ppu *ppu, struct interrupt *interrupt) {
  ppu->accumulated_dots += 4;
}

SDL_Color ppu_get_bg_pixel_color(const struct ppu *ppu, uint8_t lo, uint8_t hi,
                                 uint8_t col) {
  assert(col < 8);

  switch ((hi >> col & 1) << 1 | (lo >> col & 1)) {
  case 0:
    return PPU_COLOR_PALETTE[ppu->bgp & BGP_COLOR_IDX_0];
  case 1:
    return PPU_COLOR_PALETTE[(ppu->bgp & BGP_COLOR_IDX_1) >> 2];
  case 2:
    return PPU_COLOR_PALETTE[(ppu->bgp & BGP_COLOR_IDX_2) >> 4];
  case 3:
    return PPU_COLOR_PALETTE[(ppu->bgp & BGP_COLOR_IDX_3) >> 6];
  default:
    assert(false);
  }
}

uint8_t ppu_read_vram(const struct ppu *ppu, uint16_t a16) {
  return ppu->vram[a16 - 0x8000];
}

uint8_t ppu_read_oam(const struct ppu *ppu, uint16_t a16) {
  return ppu->oam[a16 - 0xFE00];
}

void ppu_write_vram(struct ppu *ppu, uint16_t a16, uint8_t u8) {
  ppu->vram[a16 - 0x8000] = u8;
}

void ppu_write_oam(struct ppu *ppu, uint16_t a16, uint8_t u8) {
  ppu->oam[a16 - 0xFE00] = u8;
}
