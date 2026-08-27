#pragma once
#include <stddef.h>
#include <stdint.h>

struct u16_stk {
  uint16_t *data;
  size_t size;
  size_t capacity;
};

void u16_stk_create(struct u16_stk *stk, size_t initial_cap);
void u16_stk_destroy(struct u16_stk *stk);

[[nodiscard]] bool u16_stk_contains(struct u16_stk *stk, uint16_t u16);
void u16_stk_push(struct u16_stk *stk, uint16_t u16);
void u16_stk_pop(struct u16_stk *stk);