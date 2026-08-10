#include "vector.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct u8_fixed_vec create_u8_fixed_vec(size_t capacity) {
  struct u8_fixed_vec vec = {.data = NULL, .capacity = 0};
  if (capacity == 0) {
    return vec;
  }

  uint8_t *buf = malloc(capacity * sizeof(uint8_t));
  if (buf) {
    vec.data = buf;
    vec.capacity = capacity;
  }
  return vec;
}

struct u8_fixed_vec create_u8_fixed_vec_from_file(size_t capacity, FILE *file) {
  struct u8_fixed_vec vec = {.data = NULL, .capacity = 0};

  uint8_t *file_data = malloc(capacity * sizeof(uint8_t));
  if (!file_data) {
    perror("file data malloc failed");
    return vec;
  }

  bool file_read_success = true;
  rewind(file);
  fread(file_data, sizeof(uint8_t), capacity, file);
  if (feof(file)) {
    perror("EOF error");
    file_read_success = false;
  }
  if (ferror(file)) {
    perror("File error");
    file_read_success = false;
  }

  if (file_read_success) {
    vec.data = file_data;
    vec.capacity = capacity;
  }
  return vec;
}

void destroy_u8_fixed_vec(struct u8_fixed_vec *vec) {
  vec->capacity = 0;
  if (vec->data) {
    free(vec->data);
    vec->data = NULL;
  }
}

struct u16_dynamic_vec create_u16_dynamic_vec(size_t capacity) {
  struct u16_dynamic_vec vec = {.data = NULL, .size = 0, .capacity = 0};
  if (capacity == 0) {
    fprintf(stderr, "Cannot create u16_dynamic_vec of size 0\n");
    return vec;
  }

  uint16_t *buf = malloc(capacity * sizeof(uint16_t));
  if (buf) {
    vec.data = buf;
    vec.capacity = capacity;
  } else {
    fprintf(stderr, "Failed to malloc u16_dynamic_vec data\n");
  }
  return vec;
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
