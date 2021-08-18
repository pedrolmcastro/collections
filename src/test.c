#include <stdio.h>

#include "deque.h"


int compare(const void *first, const void *second) {
    return *((int *)first) - *((int *)second);
}


int main() {
    deque_t *deque = deque_construct(sizeof(int), NULL, compare);
    printf("%p\n", deque);
    deque_free(deque);

    return 0;
}