#include "u8_buf.h"
#include <stdlib.h>

void u8_buf_create(struct u8_buf *buf, size_t cap) {
  buf->data = malloc(buf->cap = cap);
}

void u8_buf_destroy(struct u8_buf *buf) {
  buf->cap = 0;
  if (buf->data)
    free(buf->data);
}
