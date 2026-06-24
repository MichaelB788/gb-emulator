#pragma once
#include "instructions.h"
#include <stdint.h>

static int (*const unprefixed_ins[256])(struct gameboy *) = {

    /// Block 0

    // TODO: 1
    [0x00] = &nop,

    [0x01] = &ld_r16_n16,
    [0x11] = &ld_r16_n16,
    [0x21] = &ld_r16_n16,
    [0x31] = &ld_r16_n16,

    [0x02] = &ld_r16_ind_a,
    [0x12] = &ld_r16_ind_a,
    [0x22] = &ld_r16_ind_a,
    [0x32] = &ld_r16_ind_a,

    [0x03] = &inc_r16,
    [0x13] = &inc_r16,
    [0x23] = &inc_r16,
    [0x33] = &inc_r16,

    [0x04] = &inc_r8,
    [0x14] = &inc_r8,
    [0x24] = &inc_r8,
    [0x34] = &inc_hl_ind,

    [0x05] = &dec_r8,
    [0x15] = &dec_r8,
    [0x25] = &dec_r8,
    [0x35] = &dec_hl_ind,

    [0x06] = &ld_r8_n8,
    [0x16] = &ld_r8_n8,
    [0x26] = &ld_r8_n8,
    [0x36] = &ld_hl_ind_n8,

    // TODO: 7, 8

    [0x09] = &add_hl_r16,
    [0x19] = &add_hl_r16,
    [0x29] = &add_hl_r16,
    [0x39] = &add_hl_r16,

    [0x0A] = &ld_a_r16_ind,
    [0x1A] = &ld_a_r16_ind,
    [0x2A] = &ld_a_r16_ind,
    [0x3A] = &ld_a_r16_ind,

    [0x0B] = &dec_r16,
    [0x1B] = &dec_r16,
    [0x2B] = &dec_r16,
    [0x3B] = &dec_r16,

    [0x0C] = &inc_r8,
    [0x1C] = &inc_r8,
    [0x2C] = &inc_r8,
    [0x3C] = &inc_r8,

    [0x0D] = &dec_r8,
    [0x1D] = &dec_r8,
    [0x2D] = &dec_r8,
    [0x3D] = &dec_r8,

    [0x0E] = &ld_r8_n8,
    [0x1E] = &ld_r8_n8,
    [0x2E] = &ld_r8_n8,
    [0x3E] = &ld_r8_n8,

    // TODO: F

    /// Block 1: 8-bit register-to-register loads

    [0x40] = &ld_r8_r8,
    [0x41] = &ld_r8_r8,
    [0x42] = &ld_r8_r8,
    [0x43] = &ld_r8_r8,
    [0x44] = &ld_r8_r8,
    [0x45] = &ld_r8_r8,
    [0x46] = &ld_r8_hl_ind,
    [0x47] = &ld_r8_r8,
    [0x48] = &ld_r8_r8,
    [0x49] = &ld_r8_r8,
    [0x4A] = &ld_r8_r8,
    [0x4B] = &ld_r8_r8,
    [0x4C] = &ld_r8_r8,
    [0x4D] = &ld_r8_r8,
    [0x4E] = &ld_r8_hl_ind,
    [0x4F] = &ld_r8_r8,
    [0x50] = &ld_r8_r8,
    [0x51] = &ld_r8_r8,
    [0x52] = &ld_r8_r8,
    [0x53] = &ld_r8_r8,
    [0x54] = &ld_r8_r8,
    [0x55] = &ld_r8_r8,
    [0x56] = &ld_r8_hl_ind,
    [0x57] = &ld_r8_r8,
    [0x58] = &ld_r8_r8,
    [0x59] = &ld_r8_r8,
    [0x5A] = &ld_r8_r8,
    [0x5B] = &ld_r8_r8,
    [0x5C] = &ld_r8_r8,
    [0x5D] = &ld_r8_r8,
    [0x5E] = &ld_r8_hl_ind,
    [0x5F] = &ld_r8_r8,
    [0x60] = &ld_r8_r8,
    [0x61] = &ld_r8_r8,
    [0x62] = &ld_r8_r8,
    [0x63] = &ld_r8_r8,
    [0x64] = &ld_r8_r8,
    [0x65] = &ld_r8_r8,
    [0x66] = &ld_r8_hl_ind,
    [0x67] = &ld_r8_r8,
    [0x68] = &ld_r8_r8,
    [0x69] = &ld_r8_r8,
    [0x6A] = &ld_r8_r8,
    [0x6B] = &ld_r8_r8,
    [0x6C] = &ld_r8_r8,
    [0x6D] = &ld_r8_r8,
    [0x6E] = &ld_r8_hl_ind,
    [0x6F] = &ld_r8_r8,
    [0x70] = &ld_hl_ind_r8,
    [0x71] = &ld_hl_ind_r8,
    [0x72] = &ld_hl_ind_r8,
    [0x73] = &ld_hl_ind_r8,
    [0x74] = &ld_hl_ind_r8,
    [0x75] = &ld_hl_ind_r8,
    [0x76] = &halt,
    [0x77] = &ld_hl_ind_r8,
    [0x78] = &ld_r8_r8,
    [0x79] = &ld_r8_r8,
    [0x7A] = &ld_r8_r8,
    [0x7B] = &ld_r8_r8,
    [0x7C] = &ld_r8_r8,
    [0x7D] = &ld_r8_r8,
    [0x7E] = &ld_r8_hl_ind,
    [0x7F] = &ld_r8_r8,

    /// Block 2: 8-bit arithmetic

    [0xE0] = &ldh_n8_ind_a,
    [0xF0] = &ldh_a_n8_ind,

    [0xC1] = &pop_r16stk,
    [0xD1] = &pop_r16stk,
    [0xE1] = &pop_r16stk,
    [0xF1] = &pop_r16stk,

    [0xE2] = &ldh_c_ind_a,
    [0xF2] = &ldh_a_c_ind,

    [0xC5] = &push_r16stk,
    [0xD5] = &push_r16stk,
    [0xE5] = &push_r16stk,
    [0xF5] = &push_r16stk,

    [0xC6] = &add_n8,
    [0xD6] = &sub_n8,
    //[0xE6] = &and_n8,
    //[0xF6] = &or_n8,

    [0xC7] = &rst_vec,
    [0xD7] = &rst_vec,
    [0xE7] = &rst_vec,
    [0xF7] = &rst_vec,

    [0xEA] = &ld_n16_ind_a,
    [0xFA] = &ld_a_n16_ind,

    [0xCE] = &adc_n8,
    [0xDE] = &sbc_n8,
    //[0xEE] = &xor_n8,
    [0xFE] = &cp_n8,

    [0xCF] = &rst_vec,
    [0xDF] = &rst_vec,
    [0xEF] = &rst_vec,
    [0xFF] = &rst_vec,
};
