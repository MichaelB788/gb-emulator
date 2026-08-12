#include "cartridge.h"
#include "constants.h"
#include "mapper.h"
#include "vector.h"
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/// This function will free the memory in cart and rom_file and print the msg to
/// stderr
static bool cartridge_fail(struct cartridge *cart, FILE *rom_file,
                           const char *msg) {
  fprintf(stderr, "Failed to create cartridge: %s\n", msg);
  fclose(rom_file);
  destroy_cartridge(cart);
  return false;
}

bool create_cartridge(struct cartridge *cart, const char *path_to_rom) {
  bool success = true;

  FILE *rom_file = fopen(path_to_rom, "rb");
  if (!rom_file) {
    perror("Could not open file");
    return cartridge_fail(cart, rom_file, strerror(errno));
  }

  uint8_t header[0x50] = {0};
  if (fseek(rom_file, 0x100, SEEK_SET) == 0) {
    char *buf;
    fread(header, 1, 0x50, rom_file);
    if (ferror(rom_file) || feof(rom_file)) {
      return cartridge_fail(cart, rom_file, strerror(errno));
    }
  } else {
    return cartridge_fail(cart, rom_file, strerror(errno));
  }

  if (!create_mapper(&cart->mapper, header[0x47])) {
    return cartridge_fail(cart, rom_file, "Could not create mapper");
  }

  if (!create_u8_fixed_vec_from_file(&cart->rom, rom_file,
                                     KiB_32 * (1 << header[0x48]))) {
    return cartridge_fail(cart, rom_file, "Could not create ROM");
  }

  const size_t ram_capacities[] = {0ul, 0ul, KiB_8, KiB_32, KiB_128, KiB_64};
  const size_t ram_capacity = ram_capacities[header[0x49]];
  if (ram_capacity > 0 && !create_u8_fixed_vec(&cart->ram, ram_capacity)) {
    return cartridge_fail(cart, rom_file, "Could not create RAM");
  }

  fclose(rom_file);
  return true;
}

void destroy_cartridge(struct cartridge *cart) {
  destroy_u8_fixed_vec(&cart->rom);
  destroy_u8_fixed_vec(&cart->ram);
}

uint8_t cartridge_read_rom(struct cartridge *cart, uint16_t addr) {
  return mapper_read_rom(&cart->mapper, &cart->rom, addr);
}

void cartridge_write_rom(struct cartridge *cart, uint16_t addr, uint8_t val) {
  mapper_write_rom(&cart->mapper, &cart->rom, addr, val);
}

uint8_t cartridge_read_ram(struct cartridge *cart, uint16_t addr) {
  return mapper_read_ram(&cart->mapper, &cart->ram, addr);
}

void cartridge_write_ram(struct cartridge *cart, uint16_t addr, uint8_t val) {
  mapper_write_ram(&cart->mapper, &cart->ram, addr, val);
}
