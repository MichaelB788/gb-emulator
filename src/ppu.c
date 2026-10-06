#include "ppu.h"

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
