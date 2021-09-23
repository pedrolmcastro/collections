#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdbool.h>


typedef struct _Vector vector_t;
extern const size_t VECTOR_LIMIT;


vector_t *vector_cosntruct(size_t width, size_t limit, size_t capacity, double increment, void (*free_data)(void *data));

void vector_free(vector_t *vector);
bool vector_clear(vector_t *vector);

bool vector_reserve(vector_t *vector, size_t size);
bool vector_trim(vector_t *vector);

bool vector_insert(vector_t *vector, const void *data, size_t index);

bool vector_get(vector_t *vector, void *destination, size_t index);
bool vector_set(vector_t *vector, const void *data, size_t index);

size_t vector_size(vector_t *vector);
size_t vector_width(vector_t *vector);
size_t vector_limit(vector_t *vector);
size_t vector_capacity(vector_t *vector);

bool vector_empty(vector_t *vector);
bool vector_full(vector_t *vector);

#endif // VECTOR_H
