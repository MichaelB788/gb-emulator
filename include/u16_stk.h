#pragma once
#include <stddef.h>
#include <stdint.h>

static constexpr size_t U16_STK_CAPACITY = 20;
struct u16_stk {
  size_t size;
  uint16_t data[U16_STK_CAPACITY];
};

void u16_stk_init(struct u16_stk *stk);

[[nodiscard]] bool u16_stk_contains(const struct u16_stk *stk, uint16_t u16);

bool u16_stk_push(struct u16_stk *stk, uint16_t u16);
bool u16_stk_pop(struct u16_stk *stk);
