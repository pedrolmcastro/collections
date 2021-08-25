#include <stdio.h>

#include "vector.h"


struct _Vector {
    void **data;
    size_t width;
    size_t size;
    size_t capacity;
    double increment;
    void (*free_data)(void *data);
    int (*compare)(const void *first, const void *second);
};
