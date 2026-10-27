#pragma once
#include "lcd.h"
#include <SDL3/SDL_pixels.h>
#include <stdint.h>

struct interrupt;

struct ppu {
  unsigned accumulated_dots;

  uint8_t dma; // 0xFF46: OAM DMA source address & start

  uint8_t vram[0x2000]; // video RAM, [0x8000, 0x9FFF]
  uint8_t oam[160];     // object attribute memory, [0xFE00, 0xFE9F]

  struct lcd lcd;
};

void ppu_init(struct ppu *ppu);

void ppu_tick(struct ppu *ppu, struct interrupt *interrupt);

uint8_t ppu_read_vram(const struct ppu *ppu, uint16_t a16);
uint8_t ppu_read_oam(const struct ppu *ppu, uint16_t a16);

void ppu_write_vram(struct ppu *ppu, uint16_t a16, uint8_t u8);
void ppu_write_oam(struct ppu *ppu, uint16_t a16, uint8_t u8);
