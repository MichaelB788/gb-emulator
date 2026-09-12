#pragma once
#include <stddef.h>
#include <stdint.h>

// Memory sizes

#define KiB(n) ((n) * 1024ul)
#define MiB(n) ((n) * 1024ul * 1024ul)

// Memory map

static constexpr uint16_t ROM_END = 0x7FFF;

static constexpr uint16_t VRAM_BEGIN = 0x8000;
static constexpr uint16_t VRAM_END = 0x9FFF;

static constexpr uint16_t EXRAM_BEGIN = 0xA000;
static constexpr uint16_t EXRAM_END = 0xBFFF;

static constexpr uint16_t WRAM_BEGIN = 0xC000;
static constexpr uint16_t WRAM_END = 0xDFFF;

static constexpr uint16_t ECHO_RAM_BEGIN = 0xE000;
static constexpr uint16_t ECHO_RAM_END = 0xFDFF;

static constexpr uint16_t OAM_BEGIN = 0xFE00;
static constexpr uint16_t OAM_END = 0xFE9F;

static constexpr uint16_t PROHIBIT_BEGIN = 0xFEA0;
static constexpr uint16_t PROHIBIT_END = 0xFEFF;

static constexpr uint16_t IO_REGISTERS_BEGIN = 0xFF00;

static constexpr uint16_t IO_JOYPAD = 0xFF00;

static constexpr uint16_t IO_SB = 0xFF01;
static constexpr uint16_t IO_SC = 0xFF02;

static constexpr uint16_t IO_DIV = 0xFF04;
static constexpr uint16_t IO_TIMA = 0xFF05;
static constexpr uint16_t IO_TMA = 0xFF06;
static constexpr uint16_t IO_TAC = 0xFF07;

static constexpr uint16_t IO_IF = 0xFF0F;

static constexpr uint16_t IO_LCDC = 0xFF40;
static constexpr uint16_t IO_STAT = 0xFF41;
static constexpr uint16_t IO_LY = 0xFF44;
static constexpr uint16_t IO_LYC = 0xFF45;

static constexpr uint16_t IO_REGISTERS_END = 0xFF7F;

static constexpr uint16_t HRAM_BEGIN = 0xFF80;
static constexpr uint16_t HRAM_END = 0xFFFE;

static constexpr uint16_t IO_IE = 0xFFFF;

// CPU clock freq

static constexpr size_t CPU_CLOCK_HZ = 4194304;
