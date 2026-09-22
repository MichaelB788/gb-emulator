#include "bus.h"
#include "cartridge.h"
#include "constants.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

[[nodiscard]] static uint8_t bus_read_io(const struct bus *bus, uint16_t addr) {
  switch (addr) {
    // clang-format off
  case 0xFF00: return bus->JOYP;
  case 0xFF01: return bus->SB;
  case 0xFF02: return bus->SC;
  case 0xFF04: return bus->system_counter >> 8 & 0xFF;
  case 0xFF05: return bus->TIMA;
  case 0xFF06: return bus->TMA;
  case 0xFF07: return bus->TAC;
  case 0xFF0F: return bus->IF;
  case 0xFFFF: return bus->IE;
  case 0xFF44: return 0x90;
  default: return 0xFF;
    // clang-format on
  }
}

static void bus_write_io(struct bus *bus, uint16_t addr, uint8_t val) {
  switch (addr) {
    // clang-format off
  case 0xFF00: bus->JOYP = (bus->JOYP & ~0b110000) | val & 0x30; break;
  case 0xFF01: bus->SB = val; break;
  case 0xFF02: if ((bus->SC = val & 0x81) == 0x81) putchar(bus->SB); fflush(stdout); break;
  case 0xFF04: bus->system_counter = 0; break;
  case 0xFF05: bus->TIMA = val; break;
  case 0xFF06: bus->TMA = val; break;
  case 0xFF07: bus->TAC = val & 0x7; break;
  case 0xFF0F: bus->IF = val & 0x1F; break;
  case 0xFFFF: bus->IE = val & 0x1F; break;
  default: break;
    // clang-format on
  }
}

void bus_init(struct bus *bus, struct cartridge *cart) {
  assert(cart != nullptr);
  bus->cart = cart;
  bus->JOYP = 0x3F;
  bus->SB = bus->SC = 0;
  bus->elapsed_t_cycles = bus->system_counter = 0;
  bus->TIMA = bus->TMA = bus->TAC = 0;
  bus->IF = bus->IE = 0;
}

static void timer_tick(struct bus *bus) {
  static constexpr unsigned FREQS[] = {
      CPU_CLOCK_HZ / 4096, CPU_CLOCK_HZ / 262144, CPU_CLOCK_HZ / 65536,
      CPU_CLOCK_HZ / 16384};

  bus->system_counter += 4;
  if (bus->TAC & 0x4) {
    bus->elapsed_t_cycles += 4;
    const uint8_t clk_sel = bus->TAC & 0x3;
    if (bus->elapsed_t_cycles >= FREQS[clk_sel]) {
      if (++bus->TIMA == 0) {
        bus->TIMA = bus->TMA;
        bus->IF |= INTERRUPT_TIMER;
      }
      bus->elapsed_t_cycles -= FREQS[clk_sel];
    }
  }
}

void bus_tick(struct bus *bus) { timer_tick(bus); }

uint8_t bus_read_byte(const struct bus *bus, uint16_t addr) {
  // clang-format off
  if (0x0000 <= addr && addr <= 0x7FFF) return cartridge_read_rom(bus->cart, addr);
  if (0x8000 <= addr && addr <= 0x9FFF) return bus->vram[addr - 0x8000];
  if (0xA000 <= addr && addr <= 0xBFFF) return cartridge_read_ram(bus->cart, addr);
  if (0xC000 <= addr && addr <= 0xDFFF) return bus->wram[addr - 0xC000];
  if (0xE000 <= addr && addr <= 0xFDFF) return bus->wram[addr - 0xE000];
  if (0xFE00 <= addr && addr <= 0xFE9F) return 0xFF;
  if (0xFEA0 <= addr && addr <= 0xFEFF) return 0xFF;
  if (0xFF00 <= addr && addr <= 0xFF7F || addr == 0xFFFF) return bus_read_io(bus, addr);
  if (0xFF80 <= addr && addr <= 0xFFFE) return bus->hram[addr - 0xFF80];
  unreachable();
  // clang-format on
}

void bus_write_byte(struct bus *bus, uint16_t addr, uint8_t val) {
  // clang-format off
  if (0x0000 <= addr && addr <= 0x7FFF) cartridge_write_rom(bus->cart, addr, val);
  else if (0x8000 <= addr && addr <= 0x9FFF) bus->vram[addr - 0x8000] = val;
  else if (0xA000 <= addr && addr <= 0xBFFF) cartridge_write_ram(bus->cart, addr, val);
  else if (0xC000 <= addr && addr <= 0xDFFF) bus->wram[addr - 0xC000] = val;
  else if (0xE000 <= addr && addr <= 0xFDFF) bus->wram[addr - 0xE000] = val;
  else if (0xFE00 <= addr && addr <= 0xFE9F) return;
  else if (0xFEA0 <= addr && addr <= 0xFEFF) return;
  else if (0xFF00 <= addr && addr <= 0xFF7F || addr == 0xFFFF) bus_write_io(bus, addr, val);
  else if (0xFF80 <= addr && addr <= 0xFFFE) bus->hram[addr - 0xFF80] = val;
  unreachable();
  // clang-format on
}
