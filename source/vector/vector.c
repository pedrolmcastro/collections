/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file vector.c
 * 
 * @brief Generic vector implementation
 */


#include "vector.h"

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


struct _Vector {
    size_t size;
    size_t width;
    size_t limit;
    size_t capacity;
    double growth;
    void **values;
    void (*free_)(void *value);
    bool (*copy)(const void *source, void *destination);
};


const size_t VECTOR_LIMIT = SIZE_MAX / (2 * sizeof(void *)) - 1;


// Auxiliary functions for values
static void *_value_construct(vector_t *vector, const void *value);
static bool _value_copy(vector_t *vector, const void *value, void *destination);
static void _value_free(vector_t *vector, void *value);

// Auxiliary functions and variables for sorting
static int _qsort_compare(const void *first, const void *second);
static int (*_compare)(const void *first, const void *second);
static bool _reverse;


size_t vector_capacity(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->capacity;
}

bool vector_clear(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < vector->size; i++) {
        _value_free(vector, vector->values[i]);
    }

    vector->size = 0;

    return true;
}

vector_t *vector_construct(size_t width, size_t limit, size_t capacity, double growth, bool (*copy)(const void *source, void *destination), void (*free_)(void *value)) {
    if (width == 0 || limit == 0 || limit > VECTOR_LIMIT || capacity > limit || growth < 2) {
        errno = EINVAL;
        return NULL;
    }

    vector_t *vector = malloc(sizeof(vector_t));
    if (vector == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (capacity > 0) {
        vector->values = malloc(capacity * sizeof(void *));
        if (vector->values == NULL) {
            vector_free(vector);
            errno = ENOMEM;
            return NULL;
        }
    }
    else {
        vector->values = NULL;
    }

    vector->size = 0;

    vector->width = width;
    vector->limit = limit;
    vector->capacity = capacity;
    vector->growth = growth;
    vector->copy = copy;
    vector->free_ = free_;

    return vector;
}

bool vector_contains(vector_t *vector, const void *key, int (*compare)(const void *value, const void *key)) {
    // errno set in vector_search()
    return vector_search(vector, key, compare) != SIZE_MAX;
}

vector_t *vector_copy(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return NULL;
    }

    vector_t *copy = vector_construct(vector->width, vector->limit, vector->capacity, vector->growth, vector->copy, vector->free_);
    if (copy == NULL) {
        // errno set in vector_construct()
        return NULL;
    }

    for (size_t i = 0; i < vector->size; i++) {
        if (vector_insert(copy, copy->size, vector->values[i]) == false) {
            vector_free(copy);
            // errno set in vector_insert()
            return NULL;
        }
    }

    return copy;
}

bool vector_empty(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    return vector->size == 0;
}

void vector_free(vector_t *vector) {
    if (vector != NULL) {
        if (vector->values != NULL) {
            vector_clear(vector);
            vector_trim(vector);
        }
        
        free(vector);
    }
}

bool vector_full(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    return vector->size == vector->limit;
}

bool vector_get(vector_t *vector, size_t index, void *destination) {
    if (vector == NULL || index >= vector->size || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    // errno set in _value_copy()
    return _value_copy(vector, vector->values[index], destination);
}

double vector_growth(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->growth;
}

bool vector_insert(vector_t *vector, size_t index, const void *value) {
    if (vector == NULL || index > vector->size || value == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector_full(vector)) {
        errno = ENOSPC;
        return false;
    }

    if (vector_reserve(vector, vector->size + 1) == false) {
        // errno set in vector_reserve()
        return false;
    }

    void *new = _value_construct(vector, value);
    if (new == NULL) {
        // errno set in _value_construct()
        return false;
    }

    for (size_t i = vector->size; i > index; i--) {
        vector->values[i] = vector->values[i - 1];
    }

    vector->values[index] = new;
    vector->size++;

    return true;
}

size_t vector_limit(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->limit;
}

bool vector_remove(vector_t *vector, size_t index, void *destination) {
    if (vector == NULL || index >= vector->size) {
        errno = EINVAL;
        return false;
    }

    if (destination != NULL && vector_get(vector, index, destination) == false) {
        // errno set in vector_get()
        return false;
    }

    _value_free(vector, vector->values[index]);
    vector->size--;

    for (size_t i = index; i < vector->size; i++) {
        vector->values[i] = vector->values[i + 1];
    }

    return true;
}

bool vector_removeall(vector_t *vector, const void *remove, int (*compare)(const void *value, const void *remove)) {
    if (vector == NULL || remove == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = vector->size; i > 0; i--) {
        if (compare(vector->values[i - 1], remove) == 0) {
            vector_remove(vector, i - 1, NULL);
        }
    }

    return true;
}

bool vector_reserve(vector_t *vector, size_t size) {
    if (vector == NULL || size > vector->limit) {
        errno = EINVAL;
        return false;
    }

    if (size <= vector->capacity) {
        return true;
    }

    size_t capacity = vector->capacity == 0 ? 1 : vector->capacity;
    while (capacity < size) {
        capacity *= vector->growth;

        if (capacity > vector->limit) {
            capacity = vector->limit;
        }

        // Detect overflow
        else if (capacity <= vector->capacity) {
            capacity = vector->limit;
        }
    }

    void **values = realloc(vector->values, capacity * sizeof(void *));
    if (values == NULL) {
        errno = ENOMEM;
        return false;
    }

    vector->values = values;
    vector->capacity = capacity;

    return true;
}

vector_t *vector_reverse(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return NULL;
    }

    vector_t *reversed = vector_construct(vector->width, vector->limit, vector->capacity, vector->growth, vector->copy, vector->free_);
    if (reversed == NULL) {
        // errno set in vector_construct()
        return NULL;
    }

    for (size_t i = vector->size; i > 0; i--) {
        if (vector_insert(reversed, reversed->size, vector->values[i - 1]) == false) {
            vector_free(reversed);
            // errno set in vector_insert()
            return NULL;
        }
    }

    return reversed;
}

size_t vector_search(vector_t *vector, const void *key, int (*compare)(const void *value, const void *key)) {
    if (vector == NULL || key == NULL || compare == NULL) {
        errno = EINVAL;
        return SIZE_MAX;
    }

    for (size_t i = 0; i < vector->size; i++) {
        if (compare(vector->values[i], key) == 0) {
            return i;
        }
    }

    return SIZE_MAX;
}

bool vector_set(vector_t *vector, size_t index, const void *value) {
    if (vector == NULL || index >= vector->size || value == NULL) {
        errno = EINVAL;
        return false;
    }

    void *new = _value_construct(vector, value);
    if (new == NULL) {
        // errno set in _value_construct()
        return false;
    }

    _value_free(vector, vector->values[index]);
    vector->values[index] = new;

    return true;
}

size_t vector_size(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->size;
}

bool vector_sort(vector_t *vector, bool reverse, int (*compare)(const void *first, const void *second)) {
    if (vector == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    _reverse = reverse;
    _compare = compare;

    qsort(vector->values, vector->size, sizeof(void *), _qsort_compare);

    return true;
}

bool vector_trim(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector_empty(vector)) {
        free(vector->values);
        vector->values = NULL;
    }
    else {
        void **values = realloc(vector->values, vector->size * sizeof(void *));
        if (values == NULL) {
            errno = ENOMEM;
            return false;
        }

        vector->values = values;
    }

    vector->capacity = vector->size;

    return true;
}

size_t vector_width(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->width;
}


static void *_value_construct(vector_t *vector, const void *value) {
    if (vector == NULL || value == NULL) {
        errno = EINVAL;
        return NULL;
    }

    void *new = malloc(vector->width);
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (_value_copy(vector, value, new) == false) {
        free(new);
        // errno set in _value_copy()
        return NULL;
    }

    return new;
}

static bool _value_copy(vector_t *vector, const void *value, void *destination) {
    if (vector == NULL || value == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector->copy != NULL) {
        // errno set in copy()
        return vector->copy(value, destination);
    }

    memcpy(destination, value, vector->width);
    return true;
}

static void _value_free(vector_t *vector, void *value) {
    if (vector == NULL) {
        errno = EINVAL;
        return;
    }

    if (value != NULL) {
        if (vector->free_ != NULL) {
            vector->free_(value);
        }

        free(value);
    }
}


static int _qsort_compare(const void *first, const void *second) {
    return (_reverse ? -1 : 1) * _compare(*(void **)first, *(void **)second);
}
