#pragma once
#include "mapper.h"
#include "mbc1.h"
#include "vector.h"
#include <stdbool.h>

struct cartridge {
  enum mapper mapper;
  struct mbc1 mbc1;
  struct u8_fixed_vec rom;
  struct u8_fixed_vec ram;
};

bool cartridge_create(struct cartridge *cart, const char *path_to_rom);
void cartridge_destroy(struct cartridge *cart);
