#include "cartridge.h"
#include "byte_sizes.h"
#include "byte_vector.h"
#include "mapper.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool cart_init(struct cartridge *cart, const char *path_to_rom) {
  FILE *rom_file = fopen(path_to_rom, "rb");
  if (!rom_file) {
    perror("Could not open file");
    return false;
  }

  uint8_t header[0x150] = {0};
  if (fread(header, 1, 0x150, rom_file) != 0x150) {
    perror("Could not read file content into header");
    return false;
  }

  bool success = mapper_init(cart, header[0x147]) &&
                 byte_vector_create_from_file(
                     &cart->rom, KiB_32 * (1 << header[0x148]), rom_file);

  if (success) {
    // ram_sizes[0x01] is unused by any official ROMs, so it's unknown what RAM
    // size it should correspond to, if any.
    const size_t ram_sizes[] = {0ul, 0ul, KiB_8, KiB_32, KiB_128, KiB_64};
    const size_t ram_size = ram_sizes[header[0x149]];
    if (ram_size > 0) {
      success = byte_vector_create(&cart->ram, ram_size);
    }
  }

  fclose(rom_file);
  return success;
}

void cart_close(struct cartridge *cart) {
  byte_vector_destroy(&cart->rom);
  byte_vector_destroy(&cart->ram);
}
