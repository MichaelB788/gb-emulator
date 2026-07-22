#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct byte_vector {
  uint8_t *data;
  size_t size;
};

bool byte_vector_create(struct byte_vector *vec, size_t size);
bool byte_vector_create_from_file(struct byte_vector *vec, size_t size,
                                  FILE *file);

void byte_vector_destroy(struct byte_vector *vec);
