#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "vector.h"


struct _Vector {
    size_t width;
    size_t size;
    size_t capacity;
    double increment;
    void **elements;
    void (*free_data)(void *data);
    int (*compare)(const void *first, const void *second);
};


static void *_construct_element(const void *data, size_t width);
static void _free_element(void *element, void (*free_data)(void *data));


vector_t *vector_cosntruct(size_t width, size_t capacity, double increment, void (*free_data)(void *data), int (*compare)(const void *first, const void *second)) {
    if (width == 0 || increment <= 1 || compare == NULL) {
        errno = EINVAL;
        return NULL;
    }

    vector_t *vector = malloc(sizeof(vector_t));
    if (vector == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (capacity > 0) {
        vector->elements = malloc(capacity * sizeof(void *));
        if (vector->elements == NULL) {
            vector_free(vector);
            errno = ENOMEM;
            return NULL;
        }
    }
    else {
        vector->elements = NULL;
    }

    vector->size = 0;

    vector->width = width;
    vector->capacity = capacity;
    vector->increment = increment;
    vector->free_data = free_data;
    vector->compare = compare;

    return vector;
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

    for (size_t i = 0; i < vector_size(vector); i++) {
        _free_element(vector->elements[i], vector->free_data);
    }

    vector->size = 0;

    return true;
}


bool vector_reserve(vector_t *vector, size_t size) {
    if (vector == NULL || size <= vector_size(vector)) {
        errno = EINVAL;
        return false;
    }

    if (vector_capacity(vector) >= size) {
        return true;
    }

    size_t capacity = fmax(1, vector_capacity(vector));
    while (capacity < size) {
        capacity = fmin(capacity * vector->increment, SIZE_MAX);
    }

    void **elements = realloc(vector->elements, capacity * sizeof(void *));
    if (elements == NULL) {
        errno = ENOMEM;
        return false;
    }

    vector->elements = elements;
    vector->capacity = capacity;

    return true;
}

bool vector_trim(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector_size(vector) > 0) {
        void **elements = realloc(vector->elements, vector_size(vector) * sizeof(void *));
        if (elements == NULL) {
            errno = ENOMEM;
            return false;
        }

        vector->elements = elements;
    }
    else {
        free(vector->elements);
        vector->elements = NULL;
    }

    vector->capacity = vector_size(vector);

    return true;
}


bool vector_insert(vector_t *vector, size_t index, const void *data) {
    if (vector == NULL || index > vector_size(vector) || data == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector_isfull(vector)) {
        errno = ENOSPC;
        return false;
    }

    if (vector_reserve(vector, vector_size(vector) + 1) == false) {
        // errno set in vector_reserve()
        return false;
    }

    void *new = _construct_element(data, vector->width);
    if (new == NULL) {
        // errno set in _construct_element()
        return false;
    }

    for (size_t i = vector_size(vector); i > index; i--) {
        vector->elements[i] = vector->elements[i - 1];
    }

    vector->elements[index] = new;
    vector->size++;

    return true;
}


bool vector_get(vector_t *vector, size_t index, void *destination) {
    if (vector == NULL || index >= vector_size(vector) || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector_isempty(vector)) {
        errno = EINVAL;
        return false;
    }

    memcpy(destination, vector->elements[index], vector->width);

    return true;
}

bool vector_set(vector_t *vector, size_t index, const void *data) {
    if (vector == NULL || index >= vector_size(vector) || data == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector_isempty(vector)) {
        errno = EINVAL;
        return false;
    }

    void *new = _construct_element(data, vector->width);
    if (new == NULL) {
        // errno set in _construct_element()
        return false;
    }

    _free_element(vector->elements[index], vector->free_data);
    vector->elements[index] = new;

    return true;
}


bool vector_isempty(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    return vector->size == 0;
}

bool vector_isfull(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    return vector->size == SIZE_MAX;
}


size_t vector_size(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->size;
}

size_t vector_capacity(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return 0;
    }

    return vector->capacity;
}


static void *_construct_element(const void *data, size_t width) {
    if (data == NULL || width == 0) {
        errno = EINVAL;
        return NULL;
    }

    void *element = malloc(width);
    if (element == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    memcpy(element, data, width);

    return element;
}

static void _free_element(void *element, void (*free_data)(void *data)) {
    if (element != NULL) {
        if (free_data != NULL) {
            free_data(element);
        }

        free(element);
    }
}
