#include "cartridge.h"
#include "constants.h"
#include "mapper.h"
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

bool cartridge_create(struct cartridge *cart, const char *path_to_rom) {
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

  const uint8_t mapper_byte = header[0x47];
  if (!mapper_init(cart, mapper_byte)) {
    fprintf(stderr, "Could not initialize mapper\n");
    goto fail_exit;
  }

  const uint8_t rom_size_byte = header[0x48];
  cart->rom =
      create_u8_fixed_vec_from_file(KiB_32 * (1 << rom_size_byte), rom_file);
  if (cart->rom.data == NULL) {
    fprintf(stderr, "Could not create the ROM\n");
    goto fail_exit;
  }

  const uint8_t ram_size_byte = header[0x49];
  const size_t ram_sizes[] = {0ul, 0ul, KiB_8, KiB_32, KiB_128, KiB_64};
  if (ram_sizes[ram_size_byte] > 0) {
    cart->ram = create_u8_fixed_vec(ram_sizes[ram_size_byte]);
    if (cart->ram.data == NULL) {
      fprintf(stderr, "Could not create RAM\n");
      goto fail_exit;
    }
  }

  fclose(rom_file);
  return true;

fail_exit:
  fclose(rom_file);
  return false;
}

void cartridge_destroy(struct cartridge *cart) {
  destroy_u8_fixed_vec(&cart->rom);
  destroy_u8_fixed_vec(&cart->ram);
}
