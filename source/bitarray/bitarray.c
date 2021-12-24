/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file bitarray.c
 * 
 * @brief Bitarray implementation using bytes as buckets
 */


#include "bitarray.h"

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


typedef uint8_t byte_t;

struct _BitArray {
    size_t size;
    size_t buckets;
    byte_t *values;
};


static const size_t _BITS[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
static const size_t _BYTE = 8;


// Auxiliary functions for bytes
static byte_t _byte_sanitize(const bitarray_t *bitarray, byte_t byte);

// Auxiliary functions for bitarrays
static const bitarray_t *_bitarray_bigger(const bitarray_t *first, const bitarray_t *second);
static const bitarray_t *_bitarray_smaller(const bitarray_t *first, const bitarray_t *second);
static void _bitarray_sanitize(bitarray_t *bitarray);


bool bitarray_all(const bitarray_t *bitarray) {
    if (bitarray == NULL) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < bitarray->buckets - 1; i++) {
        if (bitarray->values[i] != 0xFF) {
            return false;
        }
    }

    return bitarray->values[bitarray->buckets - 1] == _byte_sanitize(bitarray, 0xFF);
}

bitarray_t *bitarray_and(const bitarray_t *first, const bitarray_t *second) {
    if (first == NULL || second == NULL) {
        errno = EINVAL;
        return NULL;
    }

    const bitarray_t *smaller = _bitarray_smaller(first, second);
    const bitarray_t *bigger = _bitarray_bigger(first, second);

    bitarray_t *and = bitarray_copy(smaller);
    if (and == NULL) {
        // errno set in bitarray_copy()
        return NULL;
    }

    for (size_t i = 0; i < smaller->buckets; i++) {
        and->values[i] &= bigger->values[i];
    }

    return and;
}

bool bitarray_any(const bitarray_t *bitarray) {
    if (bitarray == NULL) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < bitarray->buckets; i++) {
        if (bitarray->values[i] != 0) {
            return true;
        }
    }

    return false;
}

size_t bitarray_buckets(const bitarray_t *bitarray) {
    if (bitarray == NULL) {
        errno = EINVAL;
        return 0;
    }

    return bitarray->buckets;
}

bool bitarray_clear(bitarray_t *bitarray) {
    if (bitarray == NULL) {
        errno = EINVAL;
        return false;
    }

    memset(bitarray->values, 0, bitarray->buckets * sizeof(byte_t));

    return true;
}

bitarray_t *bitarray_construct(size_t size) {
    if (size == 0) {
        errno = EINVAL;
        return NULL;
    }

    bitarray_t *bitarray = malloc(sizeof(bitarray_t));
    if (bitarray == NULL) {
        errno = ENOMEM;
        return false;
    }

    bitarray->size = size;
    bitarray->buckets = (size - 1) / _BYTE + 1;

    bitarray->values = calloc(bitarray->buckets, sizeof(byte_t));
    if (bitarray->values == NULL) {
        bitarray_free(bitarray);
        errno = ENOMEM;
        return NULL;
    }

    return bitarray;
}

bitarray_t *bitarray_copy(const bitarray_t *bitarray) {
    if (bitarray == NULL) {
        errno = EINVAL;
        return NULL;
    }

    bitarray_t *copy = bitarray_construct(bitarray->size);
    if (copy == NULL) {
        // errno set in bitarray_construct()
        return false;
    }

    memcpy(copy->values, bitarray->values, copy->buckets * sizeof(byte_t));

    return copy;
}

size_t bitarray_count(const bitarray_t *bitarray) {
    if (bitarray == NULL) {
        errno = EINVAL;
        return 0;
    }

    size_t bits = 0;

    for (size_t i = 0; i < bitarray->buckets; i++) {
        // Count the set bits per hexadecimal
        for (byte_t j = bitarray->values[i]; j != 0; j >>= 4) {
            bits += _BITS[j & 0xF];
        }
    }

    return bits;
}

bool bitarray_fill(bitarray_t *bitarray) {
    if (bitarray == NULL) {
        errno = EINVAL;
        return false;
    }

    memset(bitarray->values, 0xFF, bitarray->buckets * sizeof(byte_t));
    _bitarray_sanitize(bitarray);

    return true;
}

bool bitarray_flip(bitarray_t *bitarray, size_t index) {
    if (bitarray == NULL || index >= bitarray->size) {
        errno = EINVAL;
        return false;
    }

    bitarray->values[index / _BYTE] ^= (1 << (index % _BYTE));

    return true;
}

void bitarray_free(bitarray_t *bitarray) {
    if (bitarray != NULL) {
        free(bitarray->values);
        free(bitarray);
    }
}

bool bitarray_none(const bitarray_t *bitarray) {
    if (bitarray == NULL) {
        errno = EINVAL;
        return false;
    }

    return !bitarray_any(bitarray);
}

bitarray_t *bitarray_not(const bitarray_t *bitarray) {
    if (bitarray == NULL) {
        errno = EINVAL;
        return NULL;
    }

    bitarray_t *not = bitarray_construct(bitarray->size);
    if (not == NULL) {
        // errno set in bitarray_construct()
        return NULL;
    }

    for (size_t i = 0; i < not->buckets; i++) {
        not->values[i] = ~bitarray->values[i];
    }
    _bitarray_sanitize(not);

    return not;
}

bitarray_t *bitarray_or(const bitarray_t *first, const bitarray_t *second) {
    if (first == NULL || second == NULL) {
        errno = EINVAL;
        return NULL;
    }

    const bitarray_t *smaller = _bitarray_smaller(first, second);
    const bitarray_t *bigger = _bitarray_bigger(first, second);

    bitarray_t *or = bitarray_copy(bigger);
    if (or == NULL) {
        // errno set in bitarray_copy()
        return NULL;
    }

    for (size_t i = 0; i < smaller->buckets; i++) {
        or->values[i] |= smaller->values[i];
    }

    return or;
}

bool bitarray_reset(bitarray_t *bitarray, size_t index) {
    if (bitarray == NULL || index >= bitarray->size) {
        errno = EINVAL;
        return false;
    }

    bitarray->values[index / _BYTE] &= ~(1 << (index % _BYTE));

    return true;
}

bool bitarray_set(bitarray_t *bitarray, size_t index) {
    if (bitarray == NULL || index >= bitarray->size) {
        errno = EINVAL;
        return false;
    }

    bitarray->values[index / _BYTE] |= (1 << (index % _BYTE));

    return true;
}

size_t bitarray_size(const bitarray_t *bitarray) {
    if (bitarray == NULL) {
        errno = EINVAL;
        return 0;
    }

    return bitarray->size;
}

bool bitarray_test(const bitarray_t *bitarray, size_t index) {
    if (bitarray == NULL || index >= bitarray->size) {
        errno = EINVAL;
        return false;
    }

    return (bitarray->values[index / _BYTE] >> (index % _BYTE)) & 1;
}

bitarray_t *bitarray_xor(const bitarray_t *first, const bitarray_t *second) {
    if (first == NULL || second == NULL) {
        errno = EINVAL;
        return NULL;
    }

    const bitarray_t *smaller = _bitarray_smaller(first, second);
    const bitarray_t *bigger = _bitarray_bigger(first, second);

    bitarray_t *xor = bitarray_copy(bigger);
    if (xor == NULL) {
        // errno set in bitarray_copy()
        return NULL;
    }

    for (size_t i = 0; i < smaller->buckets; i++) {
        xor->values[i] ^= smaller->values[i];
    }

    return xor;
}


static byte_t _byte_sanitize(const bitarray_t *bitarray, byte_t byte) {
    return byte & ~(0xFF << (bitarray->size % _BYTE));
}


static const bitarray_t *_bitarray_bigger(const bitarray_t *first, const bitarray_t *second) {
    if (first == NULL || second == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return first->size > second->size ? first : second;
}

static const bitarray_t *_bitarray_smaller(const bitarray_t *first, const bitarray_t *second) {
    if (first == NULL || second == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return first->size <= second->size ? first : second;
}

static void _bitarray_sanitize(bitarray_t *bitarray) {
    bitarray->values[bitarray->buckets - 1] = _byte_sanitize(bitarray, bitarray->values[bitarray->buckets - 1]);
}
