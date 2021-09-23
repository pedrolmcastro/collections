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
    void **elements;
    void (*free_data)(void *data);
};

const size_t VECTOR_LIMIT = SIZE_MAX / (2 * sizeof(void *)) - 1;


static void *_construct_element(const void *data, size_t width);
static void _free_element(void *element, void (*free_data)(void *data));


vector_t *vector_cosntruct(size_t width, size_t limit, size_t capacity, double increment, void (*free_data)(void *data)) {
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
    vector->limit = limit;
    vector->capacity = capacity;
    vector->increment = increment;
    vector->free_data = free_data;

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

    for (size_t i = 0; i < vector->size; i++) {
        _free_element(vector->elements[i], vector->free_data);
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

    if (vector_empty(vector)) {
        free(vector->elements);
        vector->elements = NULL;
    }
    else {
        void **elements = realloc(vector->elements, vector->size * sizeof(void *));
        if (elements == NULL) {
            errno = ENOMEM;
            return false;
        }

        vector->elements = elements;
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

    void *new = _construct_element(data, vector->width);
    if (new == NULL) {
        // errno set in _construct_element()
        return false;
    }

    for (size_t i = vector->size; i > index; i--) {
        vector->elements[i] = vector->elements[i - 1];
    }

    vector->elements[index] = new;
    vector->size++;

    return true;
}


bool vector_get(vector_t *vector, void *destination, size_t index) {
    if (vector == NULL || destination == NULL || index >= vector->size) {
        errno = EINVAL;
        return false;
    }

    memcpy(destination, vector->elements[index], vector->width);

    return true;
}

bool vector_set(vector_t *vector, const void *data, size_t index) {
    if (vector == NULL || data == NULL || index >= vector->size) {
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
