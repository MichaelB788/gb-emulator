#include "bus.h"
#include "cartridge.h"
#include "constants.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

[[nodiscard]] static uint8_t bus_read_io(const struct bus *bus, uint16_t a16) {
  switch (a16) {
    // clang-format off
  case 0xFF00: return bus->JOYP & 0x30 ? bus->JOYP : 0x3F;
  case 0xFF01: return bus->SB;
  case 0xFF02: return bus->SC;
  case 0xFF04: return bus->system_counter >> 8 & 0xFF;
  case 0xFF05: return bus->TIMA;
  case 0xFF06: return bus->TMA;
  case 0xFF07: return bus->TAC;
  case 0xFF0F: return bus->IF;
  case 0xFF44: return 0x90;
  default: return 0xFF;
    // clang-format on
  }
}

static void bus_write_io(struct bus *bus, uint16_t a16, uint8_t u8) {
  switch (a16) {
    // clang-format off
  case 0xFF00: bus->JOYP = (bus->JOYP & ~0x30) | u8 & 0x30; break;
  case 0xFF01: bus->SB = u8; break;
  case 0xFF02: if ((bus->SC = u8 & 0x81) == 0x81) putchar(bus->SB); fflush(stdout); break;
  case 0xFF04: bus->system_counter = 0; break;
  case 0xFF05: bus->TIMA = u8; break;
  case 0xFF06: bus->TMA = u8; break;
  case 0xFF07: bus->TAC = u8 & 0x7; break;
  case 0xFF0F: bus->IF = u8 & 0x1F; break;
  default: break;
    // clang-format on
  }
}

void bus_init(struct bus *bus, struct cartridge *cart) {
  assert(cart != nullptr);
  bus->cart = cart;
  bus->JOYP = 0x3F;
  bus->SB = bus->SC = 0;
  bus->timer_elapsed_cycles = bus->system_counter = 0;
  bus->TIMA = bus->TMA = bus->TAC = 0;
  bus->IF = bus->IE = 0;
}

static void timer_tick(struct bus *bus) {
  static constexpr unsigned FREQS[] = {
      CPU_CLOCK_HZ / 4096, CPU_CLOCK_HZ / 262144, CPU_CLOCK_HZ / 65536,
      CPU_CLOCK_HZ / 16384};

  bus->system_counter += 4;
  if (bus->TAC & 0x4) {
    bus->timer_elapsed_cycles += 4;
    const uint8_t clk_sel = bus->TAC & 0x3;
    if (bus->timer_elapsed_cycles >= FREQS[clk_sel]) {
      if (++bus->TIMA == 0) {
        bus->TIMA = bus->TMA;
        bus->IF |= INTERRUPT_TIMER;
      }
      bus->timer_elapsed_cycles -= FREQS[clk_sel];
    }
  }
}

void bus_tick(struct bus *bus) { timer_tick(bus); }

uint8_t bus_read(const struct bus *bus, uint16_t a16) {
  // clang-format off
  if (0x0000 <= a16 && a16 <= 0x7FFF) return cartridge_read_rom(bus->cart, a16);
  if (0x8000 <= a16 && a16 <= 0x9FFF) return bus->vram[a16 - 0x8000];
  if (0xA000 <= a16 && a16 <= 0xBFFF) return cartridge_read_ram(bus->cart, a16);
  if (0xC000 <= a16 && a16 <= 0xDFFF) return bus->wram[a16 - 0xC000];
  if (0xE000 <= a16 && a16 <= 0xFDFF) return bus->wram[a16 - 0xE000];
  if (0xFE00 <= a16 && a16 <= 0xFE9F) return 0xFF;
  if (0xFEA0 <= a16 && a16 <= 0xFEFF) return 0xFF;
  if (0xFF00 <= a16 && a16 <= 0xFF7F) return bus_read_io(bus, a16);
  if (0xFF80 <= a16 && a16 <= 0xFFFE) return bus->hram[a16 - 0xFF80];
  else return bus->IE;
  // clang-format on
}

void bus_write(struct bus *bus, uint16_t a16, uint8_t u8) {
  // clang-format off
  if (0x0000 <= a16 && a16 <= 0x7FFF) cartridge_write_rom(bus->cart, a16, u8);
  else if (0x8000 <= a16 && a16 <= 0x9FFF) bus->vram[a16 - 0x8000] = u8;
  else if (0xA000 <= a16 && a16 <= 0xBFFF) cartridge_write_ram(bus->cart, a16, u8);
  else if (0xC000 <= a16 && a16 <= 0xDFFF) bus->wram[a16 - 0xC000] = u8;
  else if (0xE000 <= a16 && a16 <= 0xFDFF) bus->wram[a16 - 0xE000] = u8;
  else if (0xFE00 <= a16 && a16 <= 0xFE9F) return;
  else if (0xFEA0 <= a16 && a16 <= 0xFEFF) return;
  else if (0xFF00 <= a16 && a16 <= 0xFF7F) bus_write_io(bus, a16, u8);
  else if (0xFF80 <= a16 && a16 <= 0xFFFE) bus->hram[a16 - 0xFF80] = u8;
  else bus->IE = u8 & 0x1F;
  // clang-format on
}
