#include "ppu.h"
#include "interrupt.h"
#include "lcd.h"
#include "oam.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void ppu_init(struct ppu *ppu) { lcd_init(&ppu->lcd); }

void ppu_tick(struct ppu *ppu, struct interrupt *interrupt) {
  ppu->accumulated_dots += 4;
}

uint8_t ppu_read_vram(const struct ppu *ppu, uint16_t a16) {
  return ppu->vram[a16 - 0x8000];
}

uint8_t ppu_read_oam(const struct ppu *ppu, uint16_t a16) {
  return oam_read(&ppu->oam, a16);
}

void ppu_write_vram(struct ppu *ppu, uint16_t a16, uint8_t u8) {
  ppu->vram[a16 - 0x8000] = u8;
}

void ppu_write_oam(struct ppu *ppu, uint16_t a16, uint8_t u8) {
  oam_write(&ppu->oam, a16, u8);
}
