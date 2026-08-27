#include "u16_stk.h"
#include <stddef.h>
#include <stdlib.h>

void u16_stk_create(struct u16_stk *stk, size_t initial_cap) {
  stk->size = 0;
  stk->data = malloc(stk->capacity = initial_cap);
}

void u16_stk_destroy(struct u16_stk *stk) {
  stk->size = stk->capacity = 0;
  if (stk->data)
    free(stk->data);
}

[[nodiscard]] bool u16_stk_contains(struct u16_stk *stk, uint16_t u16) {
  for (size_t i = 0; i < stk->size; ++i) {
    if (stk->data[i] == u16)
      return true;
  }
  return false;
}

void u16_stk_push(struct u16_stk *stk, uint16_t u16) {
  if (stk->size == stk->capacity) {
    stk->capacity *= 2;
    stk->data = realloc(stk->data, stk->capacity * 2);
  }

  stk->data[stk->size++] = u16;
}

void u16_stk_pop(struct u16_stk *stk) {
  if (stk->size > 0)
    --stk->size;
}