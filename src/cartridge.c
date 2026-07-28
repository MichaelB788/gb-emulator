#include "cartridge.h"
#include "byte_sizes.h"
#include "byte_vector.h"
#include "mapper.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

bool cart_init(struct cartridge *cart, const char *path_to_rom) {
  FILE *rom_file = fopen(path_to_rom, "rb");
  if (!rom_file) {
    perror("Could not open file");
    return false;
  }

  uint8_t header[0x50] = {0};
  if (fseek(rom_file, 0x100, SEEK_SET) == 0) {
    fread(header, 1, 0x50, rom_file);
    if (ferror(rom_file)) {
      perror("File error");
      goto fail_exit;
    }
    if (feof(rom_file)) {
      perror("EOF error");
      goto fail_exit;
    }
  } else {
    perror("File seek error");
    goto fail_exit;
  }

  // ram_sizes[0x01] is unused by any official ROMs, so it's unknown what RAM
  // size it should correspond to, if any.
  const size_t ram_sizes[] = {0ul, 0ul, KiB_8, KiB_32, KiB_128, KiB_64};
  const uint8_t mapper_byte = header[0x47];
  const uint8_t rom_size_byte = header[0x48];
  const uint8_t ram_size_byte = header[0x49];

  if (!mapper_init(cart, mapper_byte)) {
    fprintf(stderr, "Could not initialize mapper\n");
    goto fail_exit;
  }

  if (!byte_vector_create_from_file(&cart->rom, KiB_32 * (1 << rom_size_byte),
                                    rom_file)) {
    fprintf(stderr, "Could not initialize ROM\n");
    goto fail_exit;
  }

  if (mapper_has_ram(cart->mapper) &&
      !byte_vector_create(&cart->ram, ram_sizes[ram_size_byte])) {
    fprintf(stderr, "Could not initialize RAM\n");
    goto fail_exit;
  }

  fclose(rom_file);
  return true;

fail_exit:
  fclose(rom_file);
  return false;
}

void cart_close(struct cartridge *cart) {
  byte_vector_destroy(&cart->rom);
  byte_vector_destroy(&cart->ram);
}
