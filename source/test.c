#include <stdio.h>

#include "vector.h"


int compare(const void *first, const void *second) {
    return *((int *)first) - *((int *)second);
}


int main() {
    vector_t *vector = vector_cosntruct(sizeof(int), 0, 2, NULL, compare);

    for (int i = 5; i >= 1; i--) {
        vector_insert(vector, 0, &i);
    }
    for (int i = 6; i <= 10; i++) {
        vector_insert(vector, vector_size(vector), &i);
    }

    for (int i = 0; i < vector_size(vector); i++) {
        int number;
        vector_get(vector, i, &number);
        printf("%d\n", number);
    }

    for (int i = 0; i < vector_size(vector); i++) {
        int number = 1;
        vector_set(vector, i, &number);
        vector_get(vector, i, &number);
        printf("%d\n", number);
    }

    printf("%lu\n", vector_capacity(vector));

    vector_free(vector);

    return 0;
}
