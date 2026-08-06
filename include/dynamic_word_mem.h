#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct dynamic_word_mem {
  uint16_t *data;
  size_t size;
  size_t capacity;
};

bool dynamic_word_mem_create(struct dynamic_word_mem *mem,
                             size_t init_capacity);
void dynamic_word_mem_destroy(struct dynamic_word_mem *mem);

bool dynamic_word_mem_push(struct dynamic_word_mem *mem, uint16_t u16);
