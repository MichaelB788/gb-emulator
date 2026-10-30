#include "ppu.h"
#include "lcd.h"
#include "oam.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void ppu_init(struct ppu *ppu) {
  for (size_t i = 0; i < SCREEN_PIXL_H * SCREEN_PIXL_W; ++i) {
    ppu->framebuffer[i] = lcd_get_bg_pixel(&ppu->lcd, 0);
  }
}

void ppu_tick(struct ppu *ppu, struct bus *bus) {
  // TODO: lcd tick
  oam_tick(&ppu->oam, bus);
}

uint8_t ppu_read_vram(const struct ppu *ppu, uint16_t a16) {
  switch (lcd_get_ppu_mode(&ppu->lcd)) {
  case PPU_HBLANK:
  case PPU_VBLANK:
  case PPU_OAM_SCAN:
    return ppu->vram[a16 - 0x8000];
  case PPU_DRAWING:
    return 0xFF;
  }
}

uint8_t ppu_read_oam(const struct ppu *ppu, uint16_t a16) {
  switch (lcd_get_ppu_mode(&ppu->lcd)) {
  case PPU_HBLANK:
  case PPU_VBLANK:
    return oam_read_u8(&ppu->oam, a16);
  case PPU_OAM_SCAN:
  case PPU_DRAWING:
    return 0xFF;
  }
}

void ppu_write_vram(struct ppu *ppu, uint16_t a16, uint8_t u8) {
  switch (lcd_get_ppu_mode(&ppu->lcd)) {
  case PPU_HBLANK:
  case PPU_VBLANK:
  case PPU_OAM_SCAN:
    ppu->vram[a16 - 0x8000] = u8;
    break;
  case PPU_DRAWING:
    break;
  }
}

void ppu_write_oam(struct ppu *ppu, uint16_t a16, uint8_t u8) {
  switch (lcd_get_ppu_mode(&ppu->lcd)) {
  case PPU_HBLANK:
  case PPU_VBLANK:
    oam_write_u8(&ppu->oam, a16, u8);
    break;
  case PPU_OAM_SCAN:
  case PPU_DRAWING:
    break;
  }
}
