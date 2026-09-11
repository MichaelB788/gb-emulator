#include "u16_stk.h"
#include <stddef.h>
#include <stdlib.h>

bool u16_stk_contains(const struct u16_stk *stk, uint16_t u16) {
  for (size_t i = 0; i < stk->size; ++i) {
    if (stk->data[i] == u16)
      return true;
  }
  return false;
}

bool u16_stk_push(struct u16_stk *stk, uint16_t u16) {
  if (stk->size == U16_STK_CAPACITY)
    return false;

  stk->data[stk->size++] = u16;
  return true;
}

bool u16_stk_pop(struct u16_stk *stk) {
  if (stk->size > 0) {
    --stk->size;
    return true;
  } else {
    return false;
  }
}
