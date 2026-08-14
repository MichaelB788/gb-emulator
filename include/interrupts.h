#pragma once
#include <stdbool.h>
#include <stdint.h>

#define INTERRUPT_VBLANK (1 << 0)
#define INTERRUPT_LCD (1 << 1)
#define INTERRUPT_TIMER (1 << 2)
#define INTERRUPT_SERIAL (1 << 3)
#define INTERRUPT_JOYPAD (1 << 4)
#define INTERRUPT_UNUSED 0xE0

struct cpu;

struct interrupts {
  uint8_t IF; // Interrupt flag, requests interrupts
  uint8_t IE; // Interrupt enable, calls interrupts
};

uint8_t interrupts_pending(struct interrupts interrupts);

void interrupts_service_pending(struct interrupts *interrupts, struct cpu *cpu);
