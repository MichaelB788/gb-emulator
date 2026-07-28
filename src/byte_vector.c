#include "byte_vector.h"
#include <stdio.h>
#include <stdlib.h>

bool byte_vector_create(struct byte_vector *vec, size_t size) {
  if (size == 0) {
    fprintf(stderr, "Could not create byte_vector of size 0\n");
    return false;
  }

  vec->size = size;
  vec->data = malloc(vec->size);
  if (!vec->data) {
    vec->size = 0;
    perror("byte_vector data malloc failed");
    return false;
  }
  return true;
}

bool byte_vector_create_from_file(struct byte_vector *vec, size_t size,
                                  FILE *file) {
  if (!byte_vector_create(vec, size)) {
    return false;
  }

  rewind(file);
  fread(vec->data, 1, vec->size, file);
  if (feof(file)) {
    perror("EOF error");
    goto fail_exit;
  }
  if (ferror(file)) {
    perror("File error");
    goto fail_exit;
  }
  return true;

fail_exit:
  byte_vector_destroy(vec);
  return false;
}

void byte_vector_destroy(struct byte_vector *vec) {
  vec->size = 0;
  if (vec->data) {
    free(vec->data);
    vec->data = NULL;
  }
}
