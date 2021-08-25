#include <errno.h>
#include <stdlib.h>
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

bool vector_trim(vector_t *vector) {
    if (vector == NULL) {
        errno = EINVAL;
        return false;
    }

    if (vector_size(vector) > 0) {
        void **temporary = realloc(vector->elements, vector_size(vector) * sizeof(void *));
        if (temporary == NULL) {
            errno = ENOMEM;
            return false;
        }

        vector->elements = temporary;
    }
    else {
        free(vector->elements);
        vector->elements = NULL;
    }

    vector->capacity = vector_size(vector);

    return true;
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


static void _free_element(void *element, void (*free_data)(void *data)) {
    if (element != NULL) {
        if (free_data != NULL) {
            free_data(element);
        }

        free(element);
    }
}
