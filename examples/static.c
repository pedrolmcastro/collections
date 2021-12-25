/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file static.c
 * 
 * @brief Usage example with statically allocated values
 */


#include <stdio.h>
#include <stdlib.h>

#include "collections.h"


int compare(const void *first, const void *second) {
    // Casting to the correct types
    const int *first_ = (const int *)first;
    const int *second_ = (const int *)second;

    // Comparing the values
    return *first_ - *second_;
}


int main() {
    // Constructing the collection
    vector_t *vector = vector_construct(sizeof(int), VECTOR_LIMIT, 0, 2, NULL, NULL);
    if (vector == NULL) {
        perror("Constructing failed");
        return EXIT_FAILURE;
    }

    int values[] = {2, 3, 5, 1, 4};

    // Inserting values
    for (int i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        if (vector_insert(vector, vector_size(vector), &values[i]) == false) {
            perror("Insertion failed");
            vector_free(vector);
            return EXIT_FAILURE;
        }
    }

    // Sorting values
    if (vector_sort(vector, compare) == false) {
        perror("Sorting failed");
        vector_free(vector);
        return EXIT_FAILURE;
    }

    // Retreiving values
    for (size_t i = 0; i < vector_size(vector); i++) {
        int value;
        if (vector_get(vector, i, &value) == false) {
            perror("Retreiving failed");
            vector_free(vector);
            return EXIT_FAILURE;
        }
        printf("%d ", value);
    }
    putchar('\n');

    // Removing values
    while (!vector_empty(vector)) {
        if (vector_remove(vector, 0, NULL) == false) {
            perror("Removing failed");
            vector_free(vector);
            return EXIT_FAILURE;
        }
    }

    // Freeing the collection
    vector_free(vector);

    return EXIT_SUCCESS;
}
