#include "u8_array.h"
#include <stdint.h>
#include <stdlib.h>

void u8_array_create(struct u8_array *arr, size_t size) {
  arr->data = (arr->size = size) > 0 ? malloc(sizeof(uint8_t) * size) : NULL;
}

void u8_array_destroy(struct u8_array *arr) {
  arr->size = 0;
  if (arr->data) {
    free(arr->data);
    arr->data = NULL;
  }
}
