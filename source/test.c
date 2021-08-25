#include <stdio.h>

#include "vector.h"


int compare(const void *first, const void *second) {
    return *((int *)first) - *((int *)second);
}


int main() {
    vector_t *vector = vector_cosntruct(sizeof(int), 10, 2, NULL, compare);
    printf("%p\n", vector);
    vector_free(vector);

    return 0;
}
