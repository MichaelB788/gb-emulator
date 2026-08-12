#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool create_u8_fixed_vec(struct u8_fixed_vec *vec, size_t capacity) {
  destroy_u8_fixed_vec(vec);
  uint8_t *buf = malloc(capacity * sizeof(uint8_t));
  if (buf) {
    vec->data = buf;
    vec->capacity = capacity;
    return true;
  }
  return false;
}

bool create_u8_fixed_vec_from_file(struct u8_fixed_vec *vec, FILE *file,
                                   size_t capacity) {
  destroy_u8_fixed_vec(vec);
  uint8_t *file_data = malloc(capacity * sizeof(uint8_t));
  if (!file_data) {
    perror("file data malloc failed");
    return false;
  }

  bool read_from_file = true;
  rewind(file);
  fread(file_data, sizeof(uint8_t), capacity, file);
  if (feof(file)) {
    perror("EOF error");
    read_from_file = false;
  }
  if (ferror(file)) {
    perror("File error");
    read_from_file = false;
  }

  if (read_from_file) {
    vec->data = file_data;
    vec->capacity = capacity;
    return true;
  }
  return false;
}

void destroy_u8_fixed_vec(struct u8_fixed_vec *vec) {
  vec->capacity = 0;
  if (vec->data) {
    free(vec->data);
    vec->data = NULL;
  }
}

bool create_u16_dynamic_vec(struct u16_dynamic_vec *vec, size_t capacity) {
  destroy_u16_dynamic_vec(vec);

  if (capacity == 0) {
    fprintf(stderr, "Cannot create u16_dynamic_vec of size 0\n");
    return false;
  }

  uint16_t *buf = malloc(capacity * sizeof(uint16_t));
  if (buf) {
    vec->data = buf;
    vec->capacity = capacity;
    return true;
  }
  fprintf(stderr, "Failed to malloc u16_dynamic_vec data\n");
  return false;
}

void destroy_u16_dynamic_vec(struct u16_dynamic_vec *vec) {
  vec->size = 0;
  vec->capacity = 0;
  if (vec->data) {
    free(vec->data);
    vec->data = NULL;
  }
}

bool u16_dynamic_vec_push(struct u16_dynamic_vec *vec, uint16_t u16) {
  if (vec->size == vec->capacity) {
    const size_t new_cap = (vec->capacity * 2) + 1;
    uint16_t *buf_resized = realloc(vec->data, new_cap * sizeof(uint16_t));
    if (buf_resized) {
      vec->data = buf_resized;
      vec->capacity = new_cap;
    } else {
      fprintf(stderr, "Failed to realloc u16_dynamic_vec data\n");
      return false;
    }
  }
  vec->data[vec->size++] = u16;
  return true;
}

bool u16_dynamic_vec_push_unique(struct u16_dynamic_vec *vec, uint16_t u16) {
  for (size_t i = 0; i < vec->size; ++i) {
    if (vec->data[i] == u16) {
      return false;
    }
  }
  return u16_dynamic_vec_push(vec, u16);
}
