#include "bus.h"
#include "cartridge.h"
#include "constants.h"
#include "interrupts.h"
#include "joypad.h"
#include "serial.h"
#include "timer.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

[[nodiscard]] static uint8_t bus_read_io(const struct bus *bus, uint16_t addr) {
  switch (addr) {
  case JOYPAD_P1:
    return bus->joypad.P1;
  case SERIAL_SB:
    return bus->serial.SB;
  case SERIAL_SC:
    return bus->serial.SC;
  case TIMER_DIV:
    return bus->timer.DIV;
  case TIMER_TIMA:
    return bus->timer.TIMA;
  case TIMER_TMA:
    return bus->timer.TMA;
  case TIMER_TAC:
    return bus->timer.TAC;
  case INTERRUPTS_IF:
    return bus->interrupts.IF;
  case INTERRUPTS_IE:
    return bus->interrupts.IE;
  default:
    return 0xFF;
  }
}

static void bus_write_io(struct bus *bus, uint16_t addr, uint8_t val) {
  switch (addr) {
  case JOYPAD_P1:
    joypad_write(&bus->joypad, val);
    break;
  case SERIAL_SB:
    bus->serial.SB = val;
    break;
  case SERIAL_SC:
    serial_write_sc(&bus->serial, val);
    break;
  case TIMER_DIV:
    timer_write_div(&bus->timer, val);
    break;
  case TIMER_TIMA:
    bus->timer.TIMA = val;
    break;
  case TIMER_TMA:
    bus->timer.TMA = val;
    break;
  case TIMER_TAC:
    bus->timer.TAC = val;
    break;
  case INTERRUPTS_IF:
    bus->interrupts.IF = val;
    break;
  case INTERRUPTS_IE:
    bus->interrupts.IE = val;
    break;
  default:
    break;
  }
}

void bus_init(struct bus *bus, struct cartridge *cart) {
  assert(cart != nullptr);
  bus->cart = cart;
  joypad_init(&bus->joypad);
}

void bus_tick(struct bus *bus) { timer_tick(&bus->timer, &bus->interrupts); }

uint8_t bus_read_byte(const struct bus *bus, uint16_t addr) {
  if (addr <= ROM_END)
    return cartridge_read_rom(bus->cart, addr);

  if (VRAM_BEGIN <= addr && addr <= VRAM_END)
    return bus->vram[addr - VRAM_BEGIN];

  if (EXRAM_BEGIN <= addr && addr <= EXRAM_END)
    return cartridge_read_ram(bus->cart, addr);

  if (WRAM_BEGIN <= addr && addr <= WRAM_END)
    return bus->wram[addr - WRAM_BEGIN];

  if (ECHO_RAM_BEGIN <= addr && addr <= ECHO_RAM_END)
    return bus->wram[addr - ECHO_RAM_BEGIN];

  if (OAM_BEGIN <= addr && addr <= OAM_END)
    assert(false && "OAM read");

  if (PROHIBIT_BEGIN <= addr && addr <= PROHIBIT_END)
    return 0xFF;

  if (IO_REGISTERS_BEGIN <= addr && addr <= IO_REGISTERS_END ||
      addr == INTERRUPTS_IE)
    return bus_read_io(bus, addr);

  if (HRAM_BEGIN <= addr && addr <= HRAM_END)
    return bus->hram[addr - HRAM_BEGIN];

  unreachable();
}

void bus_write_byte(struct bus *bus, uint16_t addr, uint8_t val) {
  if (addr <= ROM_END)
    cartridge_write_rom(bus->cart, addr, val);

  else if (VRAM_BEGIN <= addr && addr <= VRAM_END)
    bus->vram[addr - VRAM_BEGIN] = val;

  else if (EXRAM_BEGIN <= addr && addr <= EXRAM_END)
    cartridge_write_ram(bus->cart, addr, val);

  else if (WRAM_BEGIN <= addr && addr <= WRAM_END)
    bus->wram[addr - WRAM_BEGIN] = val;

  else if (ECHO_RAM_BEGIN <= addr && addr <= ECHO_RAM_END)
    bus->wram[addr - ECHO_RAM_BEGIN] = val;

  else if (OAM_BEGIN <= addr && addr <= OAM_END)
    assert(false && "OAM write");

  else if (PROHIBIT_BEGIN <= addr && addr <= PROHIBIT_END)
    return;

  else if (IO_REGISTERS_BEGIN <= addr && addr <= IO_REGISTERS_END ||
           addr == INTERRUPTS_IE)
    bus_write_io(bus, addr, val);

  else if (HRAM_BEGIN <= addr && addr <= HRAM_END)
    bus->hram[addr - HRAM_BEGIN] = val;

  unreachable();
}
