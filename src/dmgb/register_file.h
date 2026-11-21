#pragma once
#include <stdbool.h>
#include <stdint.h>

#define FLAG_C 0x10u
#define FLAG_H 0x20u
#define FLAG_N 0x40u
#define FLAG_Z 0x80u

struct register_file {
  bool IME;
  uint8_t A, F, B, C, D, E, H, L;
  uint16_t PC, SP;
};

uint16_t rf_get_af(const struct register_file *rf);
uint16_t rf_get_bc(const struct register_file *rf);
uint16_t rf_get_de(const struct register_file *rf);
uint16_t rf_get_hl(const struct register_file *rf);

void rf_set_af(struct register_file *rf, uint16_t u16);
void rf_set_bc(struct register_file *rf, uint16_t u16);
void rf_set_de(struct register_file *rf, uint16_t u16);
void rf_set_hl(struct register_file *rf, uint16_t u16);

uint16_t rf_inc_bc(struct register_file *rf);
uint16_t rf_inc_de(struct register_file *rf);
uint16_t rf_inc_hl(struct register_file *rf);

uint16_t rf_dec_bc(struct register_file *rf);
uint16_t rf_dec_de(struct register_file *rf);
uint16_t rf_dec_hl(struct register_file *rf);
