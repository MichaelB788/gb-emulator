#include "ppu.h"
#include "lcd.h"
#include "oam.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void ppu_init(struct ppu *ppu) {
  // todo: link to an external framebuffer?
}

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
