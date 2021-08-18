#include <stdio.h>

#include "queue.h"


int compare(const void *first, const void *second) {
    return *((int *)first) - *((int *)second);
}


int main() {
    queue_t *queue = queue_construct(sizeof(int), NULL, compare);

    for (int number = 1; number <= 5; number++) {
        queue_enqueue(queue, &number);
    }

    printf("size: %lu\n", queue_size(queue));

    int destination;
    queue_peek(queue, &destination);
    printf("peek: %d\n", destination);

    while (!queue_isempty(queue)) {
        queue_dequeue(queue, &destination);
        printf("%d\n", destination);
    }

    queue_free(queue);

    return 0;
}