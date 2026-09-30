#pragma once
#include <stdint.h>

struct ppu {
  uint8_t vram[8 * 1024];
  uint8_t oam[0x100];
};

[[nodiscard]] uint8_t ppu_read_vram(const struct ppu *ppu, uint16_t a16);
[[nodiscard]] uint8_t ppu_read_oam(const struct ppu *ppu, uint16_t a16);

void ppu_write_vram(struct ppu *ppu, uint16_t a16, uint8_t u8);
void ppu_write_oam(struct ppu *ppu, uint16_t a16, uint8_t u8);
