#include "byte_vector.h"
#include <stdlib.h>

bool byte_vector_create(struct byte_vector *vec, size_t size) {
  if (size > 0) {
    vec->size = size;
    vec->data = malloc(vec->size);
    if (!vec->data) {
      vec->size = 0;
      perror("byte_vector data malloc failed");
      return false;
    } else {
      return true;
    }
  }
  return false;
}

bool byte_vector_create_from_file(struct byte_vector *vec, size_t size,
                                  FILE *file) {
  if (byte_vector_create(vec, size)) {
    rewind(file);
    if (fread(vec->data, 1, vec->size, file) == vec->size) {
      return true;
    } else {
      free(vec->data);
      vec->data = NULL;
      vec->size = 0;
      perror("Could not read file content into byte_vector data");
      return false;
    }
  }
  return false;
}

void byte_vector_destroy(struct byte_vector *vec) {
  vec->size = 0;
  if (vec->data) {
    free(vec->data);
    vec->data = NULL;
  }
}
