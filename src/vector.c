#include <stdlib.h> /* malloc, free */
#include <stdio.h> /* perror */
#include <string.h> /* memcpy */
#include <assert.h> /* assert */
#include "vector.h"

#define GROWTH_FACTOR 2

static result_t ResizeVector(vector_t *vector, size_t new_capacity);

struct vector {
  void *array;
  size_t capacity;
  size_t size;
  size_t element_size;
};

vector_t *CreateVector(size_t initial_capacity, size_t element_size) {
  vector_t *vector = (vector_t *)malloc(sizeof(vector_t));
  if (NULL == vector) {
    perror("Couldn't allocate memory for vector");
    return NULL;
  }

  vector->array = malloc(initial_capacity * element_size);
  if (NULL == vector->array) {
    perror("Couldn't allocate memory for vector");
    free(vector); vector = NULL;
    return NULL;
  }

  vector->capacity = initial_capacity;
  vector->element_size = element_size;
  vector->size = 0;

  return vector;
}

void DestoryVector(vector_t *vector) {
  assert(vector);
  free(vector->array); vector->array = NULL;
  free(vector); vector = NULL;
}

result_t AppendVector(vector_t *vector, const void *value) {
  void *end_of_array = NULL;
  assert(vector);
  assert(value);

  if (vector->size == vector->capacity) {
    if (SUCCESS != ReserveVector(vector, vector->capacity * GROWTH_FACTOR)) {
      return MEM_ALLOCATION_ERROR;
    }
  }

  end_of_array = (char *)vector->array + (vector->size * vector->element_size);
  memcpy(end_of_array, value, vector->element_size);
  ++vector->size;

  return SUCCESS;
}

void RemoveLastVector(vector_t *vector) {
  assert(vector);
  --vector->size;
}

void *GetElementVector(vector_t *vector, size_t index) {
  assert(vector);
  assert(index < vector->size);

  return (char *)vector->array + (index * vector->element_size);
}

bool_t IsEmptyVector(const vector_t *vector) {
  assert(vector);
  return (vector->size ? TRUE : FALSE);
}                    

size_t GetCapacityVector(const vector_t *vector) {
  assert(vector);
  return vector->capacity;
}

size_t GetSizeVector(const vector_t *vector) {
  assert(vector);
  return vector->size;
}

result_t ReserveVector(vector_t *vector, size_t new_capacity) {
  assert(vector);
  if (new_capacity > vector->capacity) {
    return ResizeVector(vector, new_capacity);
  }

  return SUCCESS;
}

result_t ShrinkVector(vector_t *vector) {
  assert(vector);

  return ResizeVector(vector, vector->size);
}

static result_t ResizeVector(vector_t *vector, size_t new_capacity) {
  void *tmp = realloc(vector->array, new_capacity * vector->element_size);
  if (NULL == tmp) {
    return MEM_ALLOCATION_ERROR;
  }

  vector->capacity = new_capacity;
  vector->array = tmp;

  return SUCCESS;
}
