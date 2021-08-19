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

    const int KEY = 5;
    puts(deque_contains(deque, &KEY) ? "true" : "false");

    int destination;
    deque_pop(deque, &destination);
    puts(deque_contains(deque, &KEY) ? "true" : "false");
    
    deque_free(deque);

    return 0;
}