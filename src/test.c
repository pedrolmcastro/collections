#include <stdio.h>

#include "queue.h"


int compare(const void *first, const void *second) {
    return *((int *)first) - *((int *)second);
}


int main() {
    queue_t *queue = queue_construct(sizeof(int), NULL, compare);
    printf("%p\n", queue);
    queue_free(queue);

    return 0;
}