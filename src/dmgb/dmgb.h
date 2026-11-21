#pragma once
#include "cartridge/cartridge.h"
#include "joypad.h"
#include "ppu/ppu.h"
#include "register_file.h"
#include "serial_transfer.h"
#include "timer.h"
#include <stdbool.h>
#include <stdint.h>

enum dmgb_state { DMGB_RUNNING, DMGB_HALTED, DMGB_HALT_BUG, DMGB_STOPPED };

struct dmgb {
  enum dmgb_state state;

  bool ime_pending; // setting IME has a delay

  uint8_t IF; // 0xFF0F: interrupt flags
  uint8_t IE; // 0xFFFF: interrupt enable

  struct register_file reg;

  struct joypad joypad;
  struct serial_transfer serial;
  struct timer timer;

  uint8_t wram[8 * 1024];
  uint8_t hram[127];

  struct ppu ppu;
  struct cartridge cartridge;
};

bool dmgb_create(struct dmgb *dmgb, const char *rom_path);
void dmgb_destroy(struct dmgb *dmgb);

void dmgb_step(struct dmgb *dmgb);
void dmgb_tick(struct dmgb *dmgb);

void dmgb_execute(struct dmgb *dmgb, uint8_t opcode);
void dmgb_execute_cb(struct dmgb *dmgb, uint8_t opcode);
