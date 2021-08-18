#include <stdio.h>

#include "deque.h"


int compare(const void *first, const void *second) {
    return *((int *)first) - *((int *)second);
}


int main() {
    deque_t *deque = deque_construct(sizeof(int), NULL, compare);
    
    for (int numero = 1; numero <= 5; numero++) {
        deque_unshift(deque, &numero);
    }
    while (!deque_isempty(deque)) {
        int destination;
        deque_pop(deque, &destination);
        printf("%d\n", destination);
    }
    putchar('\n');

    for (int numero = 1; numero <= 5; numero++) {
        deque_push(deque, &numero);
    }
    while (!deque_isempty(deque)) {
        int destination;
        deque_shift(deque, &destination);
        printf("%d\n", destination);
    }

    deque_free(deque);

    return 0;
}