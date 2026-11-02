#include "u8_array.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool u8_array_create(struct u8_array *arr, size_t size) {
  arr->data = NULL;
  arr->size = 0;
  if (size > 0) {
    if ((arr->data = malloc(arr->size = size)) == NULL) {
      perror("u8_array malloc");
      return false;
    }
  }
  return true;
}

void u8_array_destroy(struct u8_array *arr) {
  arr->size = 0;
  if (arr->data) {
    free(arr->data);
    arr->data = NULL;
  }
}
