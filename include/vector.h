#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct u8_fixed_vec {
  uint8_t *data;
  size_t capacity;
};

bool u8_fixed_vec_create(struct u8_fixed_vec *vec, size_t capacity);
bool u8_fixed_vec_create_from_file(struct u8_fixed_vec *vec, size_t capacity,
                                   FILE *file);
void u8_fixed_vec_destroy(struct u8_fixed_vec *vec);

struct u16_dynamic_vec {
  uint16_t *data;
  size_t size;
  size_t capacity;
};

bool u16_dynamic_vec_create(struct u16_dynamic_vec *vec, size_t capacity);
void u16_dynamic_vec_destroy(struct u16_dynamic_vec *vec);

bool u16_dynamic_vec_push(struct u16_dynamic_vec *vec, uint16_t u16);
bool u16_dynamic_vec_push_unique(struct u16_dynamic_vec *vec, uint16_t u16);
