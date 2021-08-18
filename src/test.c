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

    int key = 1;
    puts(queue_contains(queue, &key) ? "true" : "false");

    int destination;
    queue_dequeue(queue, &destination);
    puts(queue_contains(queue, &key) ? "true" : "false");

    queue_free(queue);

    return 0;
}