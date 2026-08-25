#pragma once
#include <stddef.h>
#include <stdint.h>

struct u8_buf {
  uint8_t *data;
  size_t cap;
};

void u8_buf_create(struct u8_buf *buf, size_t cap);

void u8_buf_destroy(struct u8_buf *buf);
