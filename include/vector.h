#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct u8_fixed_vec {
  uint8_t *data;
  size_t capacity;
};

// Success of this function should be checked by ensuring vec.data != NULL
struct u8_fixed_vec create_u8_fixed_vec(size_t capacity);

// Success of this function should be checked by ensuring vec.data != NULL
struct u8_fixed_vec create_u8_fixed_vec_from_file(size_t capacity, FILE *file);

void destroy_u8_fixed_vec(struct u8_fixed_vec *vec);

struct u16_dynamic_vec {
  uint16_t *data;
  size_t size;
  size_t capacity;
};

// Success of this function should be checked by ensuring vec.data != NULL
struct u16_dynamic_vec create_u16_dynamic_vec(size_t capacity);

void destroy_u16_dynamic_vec(struct u16_dynamic_vec *vec);

bool u16_dynamic_vec_push(struct u16_dynamic_vec *vec, uint16_t u16);

bool u16_dynamic_vec_push_unique(struct u16_dynamic_vec *vec, uint16_t u16);
