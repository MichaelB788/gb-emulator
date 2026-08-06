#include "dynamic_word_mem.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool dynamic_word_mem_create(struct dynamic_word_mem *mem,
                             size_t init_capacity) {
  if (init_capacity == 0) {
    fprintf(stderr, "Cannot create dynamic_word_mem of size 0\n");
    return false;
  }

  mem->capacity = init_capacity;
  uint16_t *buf = malloc(sizeof(uint16_t) * init_capacity);
  if (buf) {
    mem->data = buf;
    return true;
  } else {
    fprintf(stderr, "Failed to malloc dynamic_word_mem data\n");
    return false;
  }
}

void dynamic_word_mem_destroy(struct dynamic_word_mem *mem) {
  mem->size = 0;
  mem->capacity = 0;
  if (mem->data) {
    free(mem->data);
    mem->data = NULL;
  }
}

bool dynamic_word_mem_push(struct dynamic_word_mem *mem, uint16_t u16) {
  if (mem->size == mem->capacity) {
    mem->capacity *= 2;
    uint16_t *buf_resized =
        realloc(mem->data, sizeof(uint16_t) * mem->capacity);
    if (buf_resized) {
      mem->data = buf_resized;
    } else {
      fprintf(stderr, "Failed to realloc dynamic_word_mem data\n");
      return false;
    }
  }
  mem->data[mem->size++] = u16;
  return true;
}
