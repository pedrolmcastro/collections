#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdbool.h>


typedef struct _Vector vector_t;


vector_t *vector_cosntruct(size_t width, size_t capacity, double increment, void (*free_data)(void *data), int (*compare)(const void *first, const void *second));

void vector_free(vector_t *vector);
bool vector_clear(vector_t *vector);

bool vector_reserve(vector_t *vector, size_t size);
bool vector_trim(vector_t *vector);

bool vector_insert(vector_t *vector, size_t index, const void *data);

bool vector_get(vector_t *vector, size_t index, void *destination);
bool vector_set(vector_t *vector, size_t index, const void *data);

bool vector_isempty(vector_t *vector);
bool vector_isfull(vector_t *vector);

size_t vector_size(vector_t *vector);
size_t vector_capacity(vector_t *vector);

#endif // VECTOR_H
