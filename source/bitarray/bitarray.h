#ifndef BITARRAY_H
#define BITARRAY_H

#include <stddef.h>
#include <stdbool.h>


typedef struct _BitArray bitarray_t;


bitarray_t *bitarray_construct(size_t size);
bitarray_t *bitarray_copy(bitarray_t *bitarray);

void bitarray_free(bitarray_t *bitarray);

bool bitarray_test(bitarray_t *bitarray, size_t index);

bool bitarray_any(bitarray_t *bitarray);
bool bitarray_all(bitarray_t *bitarray);
bool bitarray_none(bitarray_t *bitarray);

bool bitarray_set(bitarray_t *bitarray, size_t index);
bool bitarray_reset(bitarray_t *bitarray, size_t index);
bool bitarray_flip(bitarray_t *bitarray, size_t index);

bitarray_t *bitarray_and(bitarray_t *first, bitarray_t *second);
bitarray_t *bitarray_or(bitarray_t *first, bitarray_t *second);
bitarray_t *bitarray_xor(bitarray_t *first, bitarray_t *second);
bitarray_t *bitarray_not(bitarray_t *bitarray);

bool bitarray_fill(bitarray_t *bitarray);
bool bitarray_clear(bitarray_t *bitarray);

size_t bitarray_count(bitarray_t *bitarray);

size_t bitarray_size(bitarray_t *bitarray);
size_t bitarray_buckets(bitarray_t *bitarray);

#endif // BITARRAY_H
