/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file dynamic.c
 * 
 * @brief Usage example with dynamically allocated values
 */


#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "collections.h"


bool copy(const void *source, void *destination) {
    // Casting to the correct types
    char **destination_ = (char **)destination;
    const char **source_ = (const char **)source;

    // Allocating new memory
    *destination_ = malloc((strlen(*source_) + 1) * sizeof(char));

    // Checking for errors
    if (*destination_ == NULL) {
        errno = ENOMEM;
        return false;
    }

    // Copying the value
    strcpy(*destination_, *source_);

    return true;
}

void free_(void *value) {
    // Casting to the correct type
    char **value_ = (char **)value;

    // Freeing the memory
    free(*value_);
}


int main() {
    // Constructing the collection
    list_t *list = list_construct(sizeof(char *), SIZE_MAX, copy, free_);
    if (list == NULL) {
        perror("Constructing failed");
        return EXIT_FAILURE;
    }

    char *value = malloc(7 * sizeof(char));
    strcpy(value, "String");

    // Inserting a value
    if (list_insert(list, 0, &value) == false) {
        perror("Insertion failed");
        list_free(list);
        free(value);
        return EXIT_FAILURE;
    }
    free(value); // Memory allocated outside of the collection must be freed

    // Retreiving a value
    if (list_get(list, 0, &value) == false) {
        perror("Retreiving failed");
        list_free(list);
        return EXIT_FAILURE;
    }
    puts(value);
    free(value); // Memory retrived from the collection must be freed

    // Freeing the collection
    list_free(list);

    return EXIT_SUCCESS;
}
