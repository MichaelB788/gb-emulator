#pragma once
#include <stdint.h>

static constexpr uint8_t INTERRUPT_VBLANK = 1 << 0;
static constexpr uint8_t INTERRUPT_LCD = 1 << 1;
static constexpr uint8_t INTERRUPT_TIMER = 1 << 2;
static constexpr uint8_t INTERRUPT_SERIAL = 1 << 3;
static constexpr uint8_t INTERRUPT_JOYPAD = 1 << 4;

struct cpu;

struct interrupts {
  uint8_t IF : 5; // Interrupt flag, requests interrupts
  uint8_t IE : 5; // Interrupt enable, calls interrupts
};

void interrupts_service_pending(struct interrupts *in, struct cpu *cpu);
