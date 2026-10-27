#pragma once
#include <SDL3/SDL_pixels.h>
#include <stdint.h>

#define SCREEN_PIXL_W 160ul
#define SCREEN_PIXL_H 144ul

struct interrupt;

enum lcdc_fields {
  // clang-format off
  LCDC_BG_AND_WINDOW_ENABLE = 1 << 0,
  LCDC_OBJ_ENABLE           = 1 << 1,
  LCDC_OBJ_SIZE             = 1 << 2,
  LCDC_BG_TILE_MAP          = 1 << 3,
  LCDC_BG_AND_WINDOW_TILES  = 1 << 4,
  LCDC_WINDOW_ENABLE        = 1 << 5,
  LCDC_WINDOW_TILE_MAP      = 1 << 6,
  LCDC_LCD_AND_PPU_ENABLE   = 1 << 7
  // clang-format on
};

enum stat_fields {
  // clang-format off
  STAT_PPU_MODE          = 0x3,    // read only
  STAT_LYC_EQUALS_LY     = 1 << 2, // read only

  STAT_MODE_0_INT_SELECT = 1 << 3, // read/write
  STAT_MODE_1_INT_SELECT = 1 << 4, // read/write
  STAT_MODE_2_INT_SELECT = 1 << 5, // read/write
  STAT_LYC_INT_SELECT    = 1 << 6  // read/write
  // clang-format on
};

// maps color indicies for a given 2-bit pixel
// to be used with: BGP, OBP0, and OBP1
enum color_idx_masks {
  COLOR_IDX_0 = 0x03, // color for 0b00
  COLOR_IDX_1 = 0x0C, // color for 0b01
  COLOR_IDX_2 = 0x30, // color for 0b10
  COLOR_IDX_3 = 0xC0  // color for 0b11
};

// reported by STAT.ppu_mode (defaults to 0 if ppu disabled)
enum ppu_mode { PPU_HBLANK = 0, PPU_VBLANK, PPU_OAM_SCAN, PPU_DRAWING };

struct ppu {
  unsigned accumulated_dots;

  uint8_t lcdc; // 0xFF40: LCD control
  uint8_t stat; // 0xFF41: LCD status
  uint8_t scy;  // 0xFF42: scroll Y (viewport Y position)
  uint8_t scx;  // 0xFF43: scroll X (viewport X position)
  uint8_t ly;   // 0xFF44: current scanline
  uint8_t lyc;  // 0xFF45: LY compare
  uint8_t bgp;  // 0xFF47: Background (and window) palette
  uint8_t obp0; // 0xFF48: OBJ palette data 0
  uint8_t obp1; // 0xFF49: OBJ palette data 1

  uint8_t vram[0x2000]; // video RAM, [0x8000, 0x9FFF]
  uint8_t oam[160];     // object attribute memory, [0xFE00, 0xFE9F]

  SDL_Color framebuffer[SCREEN_PIXL_W * SCREEN_PIXL_H];
};

void ppu_init(struct ppu *ppu);

void ppu_tick(struct ppu *ppu, struct interrupt *interrupt);

void ppu_draw_bg_pixel(struct ppu *ppu, uint8_t x, uint8_t y,
                       uint8_t color_idx);
void ppu_draw_obj_pixel(struct ppu *ppu, uint8_t x, uint8_t y,
                        uint8_t color_idx, uint8_t obp);

uint8_t ppu_read_vram(const struct ppu *ppu, uint16_t a16);
uint8_t ppu_read_oam(const struct ppu *ppu, uint16_t a16);

void ppu_write_vram(struct ppu *ppu, uint16_t a16, uint8_t u8);
void ppu_write_oam(struct ppu *ppu, uint16_t a16, uint8_t u8);
