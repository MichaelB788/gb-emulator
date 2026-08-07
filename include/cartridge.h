#pragma once
#include "mapper.h"
#include "mbc1.h"
#include "vector.h"
#include <stdbool.h>

struct cartridge {
  enum mapper mapper;
  union {
    struct mbc1 mbc1;
  };
  struct u8_fixed_vec rom;
  struct u8_fixed_vec ram;
};

bool cart_init(struct cartridge *cart, const char *path_to_rom);
void cart_close(struct cartridge *cart);
