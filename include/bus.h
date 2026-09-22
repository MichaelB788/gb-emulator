#pragma once
#include "cartridge.h"
#include <stdint.h>

enum interrupts {
  // clang-format off
  INTERRUPT_VBLANK = 1 << 0,
  INTERRUPT_LCD    = 1 << 1,
  INTERRUPT_TIMER  = 1 << 2,
  INTERRUPT_SERIAL = 1 << 3,
  INTERRUPT_JOYPAD = 1 << 4
  // clang-format on
};

struct bus {
  uint8_t JOYP; // Joypad

  uint8_t SB; // Serial transfer data
  uint8_t SC; // Serial transfer control

  unsigned timer_elapsed_cycles; // Tracks elapsed cycles for timer counting
  uint16_t system_counter;       // Hidden internal system counter
  uint8_t TIMA;                  // Timer counter
  uint8_t TMA;                   // Timer modulo
  uint8_t TAC;                   // Timer control

  uint8_t IE; // Interrupt enable
  uint8_t IF; // Interrupt flag

  struct cartridge *cart;

  uint8_t vram[8 * 1024];
  uint8_t wram[8 * 1024];
  uint8_t hram[127];
};

void bus_init(struct bus *bus, struct cartridge *cart);

void bus_tick(struct bus *bus); // Advances by 1 M-cycle / 4 T-cycles

[[nodiscard]] uint8_t bus_read(const struct bus *bus, uint16_t a16);

void bus_write(struct bus *bus, uint16_t a16, uint8_t u8);
