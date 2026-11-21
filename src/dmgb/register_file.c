#include "register_file.h"
#include <stdbool.h>
#include <stdint.h>

// clang-format off
uint16_t rf_get_af(const struct register_file *rf) { return rf->A << 8u | rf->F; }
uint16_t rf_get_bc(const struct register_file *rf) { return rf->B << 8u | rf->C; }
uint16_t rf_get_de(const struct register_file *rf) { return rf->D << 8u | rf->E; }
uint16_t rf_get_hl(const struct register_file *rf) { return rf->H << 8u | rf->L; }

void rf_set_af(struct register_file *rf, uint16_t u16) { rf->A = u16 >> 8u & 0xFFu; rf->F = u16 & 0xF0u; }
void rf_set_bc(struct register_file *rf, uint16_t u16) { rf->B = u16 >> 8u & 0xFFu; rf->C = u16 & 0xFFu; }
void rf_set_de(struct register_file *rf, uint16_t u16) { rf->D = u16 >> 8u & 0xFFu; rf->E = u16 & 0xFFu; }
void rf_set_hl(struct register_file *rf, uint16_t u16) { rf->H = u16 >> 8u & 0xFFu; rf->L = u16 & 0xFFu; }

uint16_t rf_inc_bc(struct register_file *rf) { const uint16_t r16 = rf_get_bc(rf); rf_set_bc(rf, r16 + 1); return r16; }
uint16_t rf_inc_de(struct register_file *rf) { const uint16_t r16 = rf_get_de(rf); rf_set_de(rf, r16 + 1); return r16; }
uint16_t rf_inc_hl(struct register_file *rf) { const uint16_t r16 = rf_get_hl(rf); rf_set_hl(rf, r16 + 1); return r16; }

uint16_t rf_dec_bc(struct register_file *rf) { const uint16_t r16 = rf_get_bc(rf); rf_set_bc(rf, r16 - 1); return r16; }
uint16_t rf_dec_de(struct register_file *rf) { const uint16_t r16 = rf_get_de(rf); rf_set_de(rf, r16 - 1); return r16; }
uint16_t rf_dec_hl(struct register_file *rf) { const uint16_t r16 = rf_get_hl(rf); rf_set_hl(rf, r16 - 1); return r16; }
// clang-format on
