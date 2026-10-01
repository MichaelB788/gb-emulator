#pragma once
#include <stdint.h>

// Writes `val` to all bits under `mask`
void u8_write_mask(uint8_t *u8, uint8_t mask, bool val);
