#include <stdio.h>

#include "queue.h"


int compare(const void *first, const void *second) {
    return *((int *)first) - *((int *)second);
}


int main() {
    queue_t *queue = queue_construct(sizeof(int), NULL, compare);

    puts(queue_isempty(queue) ? "true" : "false");
    puts(queue_isfull(queue) ? "true" : "false");
    printf("%lu\n", queue_size(queue));

    queue_free(queue);

    return 0;
}