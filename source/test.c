#include <stdio.h>

#include "vector.h"


int compare(const void *first, const void *second) {
    return *((int *)first) - *((int *)second);
}


int main() {
    vector_t *vector = vector_cosntruct(sizeof(int), 10, 2, NULL, compare);
    
    puts(vector_isempty(vector) ? "true" : "false");
    puts(vector_isfull(vector) ? "true" : "false");
    printf("%lu\n", vector_size(vector));
    printf("%lu\n", vector_capacity(vector));

    vector_free(vector);

    return 0;
}
