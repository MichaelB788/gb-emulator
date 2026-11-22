#include "bus.h"
#include "dmgb.h"
#include "register_file.h"
#include <stdint.h>

static uint8_t dmgb_dmgb_bus_read_io(const struct dmgb *dmgb, uint16_t a16);
static void dmgb_bus_write_io(struct dmgb *dmgb, uint16_t a16, uint8_t u8);

uint8_t dmgb_bus_read(const struct dmgb *dmgb, uint16_t a16) {
  // clang-format off
  if (0x0000 <= a16 && a16 <= 0x7FFF) return cartridge_read_rom(&dmgb->cartridge, a16);
  if (0x8000 <= a16 && a16 <= 0x9FFF) return ppu_read_vram(&dmgb->ppu, a16);
  if (0xA000 <= a16 && a16 <= 0xBFFF) return cartridge_read_ram(&dmgb->cartridge, a16);
  if (0xC000 <= a16 && a16 <= 0xDFFF) return dmgb->wram[a16 - 0xC000];
  if (0xE000 <= a16 && a16 <= 0xFDFF) return dmgb->wram[a16 - 0xE000];
  if (0xFE00 <= a16 && a16 <= 0xFE9F) return ppu_read_oam(&dmgb->ppu, a16);
  if (0xFEA0 <= a16 && a16 <= 0xFEFF) return 0xFF;
  if (0xFF00 <= a16 && a16 <= 0xFF7F) return dmgb_dmgb_bus_read_io(dmgb, a16);
  if (0xFF80 <= a16 && a16 <= 0xFFFE) return dmgb->hram[a16 - 0xFF80];
  else return dmgb->IE;
  // clang-format on
}

void dmgb_bus_write(struct dmgb *dmgb, uint16_t a16, uint8_t u8) {
  // clang-format off
  if (0x0000 <= a16 && a16 <= 0x7FFF) cartridge_write_rom(&dmgb->cartridge, a16, u8);
  else if (0x8000 <= a16 && a16 <= 0x9FFF) ppu_write_vram(&dmgb->ppu, a16, u8);
  else if (0xA000 <= a16 && a16 <= 0xBFFF) cartridge_write_ram(&dmgb->cartridge, a16, u8);
  else if (0xC000 <= a16 && a16 <= 0xDFFF) dmgb->wram[a16 - 0xC000] = u8;
  else if (0xE000 <= a16 && a16 <= 0xFDFF) dmgb->wram[a16 - 0xE000] = u8;
  else if (0xFE00 <= a16 && a16 <= 0xFE9F) ppu_write_oam(&dmgb->ppu, a16, u8);
  else if (0xFEA0 <= a16 && a16 <= 0xFEFF) return;
  else if (0xFF00 <= a16 && a16 <= 0xFF7F) dmgb_bus_write_io(dmgb, a16, u8);
  else if (0xFF80 <= a16 && a16 <= 0xFFFE) dmgb->hram[a16 - 0xFF80] = u8;
  else dmgb->IE = u8 & 0x1F;
  // clang-format on
}

uint8_t dmgb_read_u8(struct dmgb *dmgb, uint16_t a16) {
  const uint8_t ret = dmgb_bus_read(dmgb, a16);
  dmgb_tick(dmgb);
  return ret;
}

uint16_t dmgb_read_u16(struct dmgb *dmgb, uint16_t a16) {
  return dmgb_read_u8(dmgb, a16 + 1) << 8u | dmgb_read_u8(dmgb, a16);
}

uint8_t dmgb_fetch_u8(struct dmgb *dmgb) {
  return dmgb_read_u8(dmgb, dmgb->reg.PC++);
}

int8_t dmgb_fetch_s8(struct dmgb *dmgb) {
  return (int8_t)dmgb_read_u8(dmgb, dmgb->reg.PC++);
}

uint16_t dmgb_fetch_u16(struct dmgb *dmgb) {
  const uint16_t u16 = dmgb_read_u16(dmgb, dmgb->reg.PC);
  dmgb->reg.PC += 2;
  return u16;
}

void dmgb_write_u8(struct dmgb *dmgb, uint16_t a16, uint8_t u8) {
  dmgb_bus_write(dmgb, a16, u8);
  dmgb_tick(dmgb);
}

void dmgb_write_u16(struct dmgb *dmgb, uint16_t a16, uint16_t u16) {
  dmgb_write_u8(dmgb, a16, u16 & 0xFF);
  dmgb_write_u8(dmgb, a16 + 1, u16 >> 8);
}

void dmgb_push_u16(struct dmgb *dmgb, uint16_t u16) {
  dmgb_write_u16(dmgb, dmgb->reg.SP -= 2, u16);
}

uint16_t dmgb_pop_u16(struct dmgb *dmgb) {
  const uint16_t u16 = dmgb_read_u16(dmgb, dmgb->reg.SP);
  dmgb->reg.SP += 2;
  return u16;
}

void dmgb_jump(struct dmgb *dmgb, uint16_t a16, bool cond) {
  if (cond) {
    dmgb->reg.PC = a16;
    dmgb_tick(dmgb);
  }
}

void dmgb_jump_rotation(struct dmgb *dmgb, int8_t s8, bool cond) {
  if (cond) {
    dmgb->reg.PC += s8;
    dmgb_tick(dmgb);
  }
}

void dmgb_call(struct dmgb *dmgb, uint16_t a16, bool cond) {
  if (cond) {
    dmgb_push_u16(dmgb, dmgb->reg.PC);
    dmgb->reg.PC = a16;
    dmgb_tick(dmgb);
  }
}

void dmgb_return(struct dmgb *dmgb, bool cond) {
  if (cond) {
    dmgb->reg.PC = dmgb_pop_u16(dmgb);
    dmgb_tick(dmgb);
  }
}

uint8_t dmgb_dmgb_bus_read_io(const struct dmgb *dmgb, uint16_t a16) {
  switch (a16) {
  case 0xFF00: return joypad_read(&dmgb->joypad);
  case 0xFF01: return dmgb->serial.sb;
  case 0xFF02: return dmgb->serial.sc;
  case 0xFF04: return dmgb->timer.sys >> 8 & 0xFF; // DIV
  case 0xFF05: return dmgb->timer.tima;
  case 0xFF06: return dmgb->timer.tma;
  case 0xFF07: return dmgb->timer.tac;
  case 0xFF0F: return dmgb->IF;
  case 0xFF40: return dmgb->ppu.lcd.lcdc;
  case 0xFF41: return dmgb->ppu.lcd.stat;
  case 0xFF42: return dmgb->ppu.lcd.scy;
  case 0xFF43: return dmgb->ppu.lcd.scx;
  case 0xFF44: return 0x90; // stub
  case 0xFF45: return dmgb->ppu.lcd.lyc;
  case 0xFF46: return dmgb->ppu.oam.dma;
  case 0xFF47: return dmgb->ppu.lcd.bgp;
  case 0xFF48: return dmgb->ppu.lcd.obp0;
  case 0xFF49: return dmgb->ppu.lcd.obp1;
  default: return 0xFF;
  }
}

void dmgb_bus_write_io(struct dmgb *dmgb, uint16_t a16, uint8_t u8) {
  switch (a16) {
  case 0xFF00: joypad_write(&dmgb->joypad, u8); break;
  case 0xFF01: dmgb->serial.sb = u8; break;
  case 0xFF02: serial_transfer_write_control(&dmgb->serial, u8); break;
  case 0xFF04: dmgb->timer.sys = 0; break; // DIV write resets SYS to 0
  case 0xFF05: dmgb->timer.tima = u8; break;
  case 0xFF06: dmgb->timer.tma = u8; break;
  case 0xFF07: dmgb->timer.tac = u8 & 0x7; break;
  case 0xFF0F: dmgb->IF = u8 & 0x1F; break;
  case 0xFF40: dmgb->ppu.lcd.lcdc = u8; break;
  case 0xFF41: lcd_stat_write(&dmgb->ppu.lcd, u8); break;
  case 0xFF42: dmgb->ppu.lcd.scy = u8; break;
  case 0xFF43: dmgb->ppu.lcd.scx = u8; break;
  case 0xFF44: dmgb->ppu.lcd.ly = u8; break;
  case 0xFF45: dmgb->ppu.lcd.lyc = u8; break;
  case 0xFF46: oam_write_dma_u8(&dmgb->ppu.oam, u8); break;
  case 0xFF47: dmgb->ppu.lcd.bgp = u8; break;
  case 0xFF48: dmgb->ppu.lcd.obp0 = u8; break;
  case 0xFF49: dmgb->ppu.lcd.obp1 = u8; break;
  default: break;
  }
}
