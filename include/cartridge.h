#pragma once
#include "byte_vector.h"
#include "mapper.h"
#include "mbc1.h"
#include <stdbool.h>

struct cartridge {
  enum mapper mapper;
  union {
    struct mbc1 mbc1;
  };
  struct byte_vector rom;
  struct byte_vector ram;
};

bool cart_init(struct cartridge *cart, const char *path_to_rom);
void cart_close(struct cartridge *cart);
