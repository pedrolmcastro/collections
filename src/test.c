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

    queue_t *clone = queue_clone(queue);
    while (!queue_isempty(clone)) {
        int destination;
        queue_dequeue(clone, &destination);
        printf("%d\n", destination);
    }
    putchar('\n');

    queue_t *reversed = queue_reverse(queue);
    while (!queue_isempty(reversed)) {
        int destination;
        queue_dequeue(reversed, &destination);
        printf("%d\n", destination);
    }

    queue_free(queue);
    queue_free(clone);
    queue_free(reversed);

    return 0;
}