#include <stdio.h>

#include "deque.h"


int compare(const void *first, const void *second) {
    return *((int *)first) - *((int *)second);
}


int main() {
    deque_t *deque = deque_construct(sizeof(int), NULL, compare);
    for (int number = 1; number <= 5; number++) {
        deque_push(deque, &number);
    }

    deque_t *clone = deque_clone(deque);
    while (!deque_isempty(clone)) {
        int destination;
        deque_shift(clone, &destination);
        printf("%d\n", destination);
    }
    
    deque_free(deque);
    deque_free(clone);

    return 0;
}