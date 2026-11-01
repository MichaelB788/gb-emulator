#pragma once
#include <stddef.h>
#include <stdint.h>

struct u8_array {
  size_t size;
  uint8_t *data;
};

void u8_array_create(struct u8_array *arr, size_t size);
void u8_array_destroy(struct u8_array *arr);
