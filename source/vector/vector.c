#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "vector.h"


struct _Vector {
    size_t size;
    size_t width;
    size_t limit;
    size_t capacity;
    double increment;
    void **data;
    void (*free_data)(void *data);
    bool (*clone_data)(const void *source, void *destination);
};


const size_t VECTOR_LIMIT = SIZE_MAX / (2 * sizeof(void *)) - 1;
const size_t VECTOR_NOT_FOUND = SIZE_MAX;


static bool _data_clone(vector_t *vector, const void *data, void *destination);
static void *_data_construct(vector_t *vector, const void *data);
static void _data_free(vector_t *vector, void *data);

// Auxiliary function and variables for vector_sort()
static int _qsort_compare(const void *first, const void *second);
static int (*_compare)(const void *first, const void *second);
static bool _reverse;


vector_t *vector_construct(size_t width, size_t limit, size_t capacity, double increment, bool (*clone_data)(const void *source, void *destination), void (*free_data)(void *data)) {
    if (width == 0 || limit == 0 || limit > VECTOR_LIMIT || capacity > limit || increment < 1.5) {
        errno = EINVAL;
        return NULL;
    }

    vector_t *vector = malloc(sizeof(vector_t));
    if (vector == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (capacity > 0) {
        vector->data = malloc(capacity * sizeof(void *));
        if (vector->data == NULL) {
            vector_free(vector);
            errno = ENOMEM;
            return NULL;
        }
    }
    else {
        vector->data = NULL;
    }

    vector->size = 0;

    vector->width = width;
    vector->limit = limit;
    vector->capacity = capacity;
    vector->increment = increment;
    vector->free_data = free_data;
    vector->clone_data = clone_data;

    return vector;
}

vector_t *vector_clone(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return NULL;
    }

    vector_t *clone = vector_construct(vector->width, vector->limit, vector->capacity, vector->increment, vector->clone_data, vector->free_data);
    if (clone == NULL) {
        // errno set in vector_construct()
        return NULL;
    }

    for (size_t i = 0; i < vector->size; i++) {
        if (vector_insert(clone, vector->data[i], clone->size) == false) {
            vector_free(clone);
            // errno set in vector_insert()
            return NULL;
        }
    }

    return clone;
}

vector_t *vector_reverse(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return NULL;
    }

    vector_t *reversed = vector_construct(vector->width, vector->limit, vector->capacity, vector->increment, vector->clone_data, vector->free_data);
    if (reversed == NULL) {
        // errno set in vector_construct()
        return NULL;
    }

    for (size_t i = vector->size; i > 0; i--) {
        if (vector_insert(reversed, vector->data[i - 1], reversed->size) == false) {
            vector_free(reversed);
            // errno set in vector_insert()
            return NULL;
        }
    }

    return reversed;
}


void vector_free(vector_t *vector) {
    if (vector != NULL) {
        vector_clear(vector);
        vector_trim(vector);
        free(vector);
    }
}

bool vector_clear(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < vector->size; i++) {
        _data_free(vector, vector->data[i]);
    }

    vector->size = 0;

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
        capacity *= vector->increment;

        if (capacity > vector->limit) {
            capacity = vector->limit;
        }

        // Detect overflow
        else if (capacity <= vector->capacity) {
            capacity = vector->limit;
        }
    }

    void **data = realloc(vector->data, capacity * sizeof(void *));
    if (data == NULL) {
        errno = ENOMEM;
        return false;
    }

    vector->data = data;
    vector->capacity = capacity;

    return true;
}

bool vector_trim(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector_empty(vector)) {
        free(vector->data);
        vector->data = NULL;
    }
    else {
        void **data = realloc(vector->data, vector->size * sizeof(void *));
        if (data == NULL) {
            errno = ENOMEM;
            return false;
        }

        vector->data = data;
    }

    vector->capacity = vector->size;

    return true;
}


bool vector_insert(vector_t *vector, const void *data, size_t index) {
    if (vector == NULL || data == NULL || index > vector->size) {
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

    void *new = _data_construct(vector, data);
    if (new == NULL) {
        // errno set in _data_construct()
        return false;
    }

    for (size_t i = vector->size; i > index; i--) {
        vector->data[i] = vector->data[i - 1];
    }

    vector->data[index] = new;
    vector->size++;

    return true;
}

bool vector_remove(vector_t *vector, void *destination, size_t index) {
    if (vector == NULL || index >= vector->size) {
        errno = EINVAL;
        return false;
    }

    if (destination != NULL) {
        if (vector_get(vector, destination, index) == false) {
            // errno set in vector_get()
            return false;
        }
    }

    _data_free(vector, vector->data[index]);
    vector->size--;

    for (size_t i = index; i < vector->size; i++) {
        vector->data[i] = vector->data[i + 1];
    }

    return true;
}

bool vector_removeall(vector_t *vector, const void *data, int (*compare)(const void *first, const void *second)) {
    if (vector == NULL || data == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = vector->size; i > 0; i--) {
        if (compare(vector->data[i - 1], data) == 0) {
            if (vector_remove(vector, NULL, i - 1) == false) {
                // errno set in vector_remove()
                return false;
            }
        }
    }

    return true;
}


bool vector_get(vector_t *vector, void *destination, size_t index) {
    if (vector == NULL || destination == NULL || index >= vector->size) {
        errno = EINVAL;
        return false;
    }

    _data_clone(vector, vector->data[index], destination);

    return true;
}

bool vector_set(vector_t *vector, const void *data, size_t index) {
    if (vector == NULL || data == NULL || index >= vector->size) {
        errno = EINVAL;
        return false;
    }

    void *new = _data_construct(vector, data);
    if (new == NULL) {
        // errno set in _data_construct()
        return false;
    }

    _data_free(vector, vector->data[index]);
    vector->data[index] = new;

    return true;
}


bool vector_sort(vector_t *vector, int (*compare)(const void *first, const void *second), bool reverse) {
    if (vector == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    _compare = compare;
    _reverse = reverse;

    qsort(vector->data, vector->size, sizeof(void *), _qsort_compare);

    return true;
}

bool vector_contains(vector_t *vector, const void *key, int (*compare)(const void *first, const void *second)) {
    if (vector == NULL || key == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < vector->size; i++) {
        if (compare(vector->data[i], key) == 0) {
            return true;
        }
    }

    return false;
}

size_t vector_search(vector_t *vector, const void *key, int (*compare)(const void *first, const void *second)) {
    if (vector == NULL || key == NULL || compare == NULL) {
        errno = EINVAL;
        return VECTOR_NOT_FOUND;
    }

    for (size_t i = 0; i < vector->size; i++) {
        if (compare(vector->data[i], key) == 0) {
            return i;
        }
    }

    return VECTOR_NOT_FOUND;
}


size_t vector_size(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->size;
}

size_t vector_width(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->width;
}

size_t vector_limit(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->limit;
}

size_t vector_capacity(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->capacity;
}


bool vector_empty(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    return vector->size == 0;
}

bool vector_full(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    return vector->size == vector->limit;
}


static bool _data_clone(vector_t *vector, const void *data, void *destination) {
    if (vector == NULL || data == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector->clone_data != NULL) {
        return vector->clone_data(data, destination);
    }
    else {
        memcpy(destination, data, vector->width);
        return true;
    }
}

static void *_data_construct(vector_t *vector, const void *data) {
    if (vector == NULL || data == NULL) {
        errno = EINVAL;
        return NULL;
    }

    void *new = malloc(vector->width);
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (_data_clone(vector, data, new) == false) {
        _data_free(vector, new);
        // errno set in _data_clone()
        return NULL;
    }

    return new;
}

static void _data_free(vector_t *vector, void *data) {
    if (vector == NULL) {
        errno = EINVAL;
        return;
    }

    if (data != NULL) {
        if (vector->free_data != NULL) {
            vector->free_data(data);
        }

        free(data);
    }
}


static int _qsort_compare(const void *first, const void *second) {
    return (_reverse ? -1 : 1) * _compare(*(void **)first, *(void **)second);
}
