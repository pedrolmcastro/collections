#include <stdio.h>

#include "deque.h"


int compare(const void *first, const void *second) {
    return *((int *)first) - *((int *)second);
}


int main() {
    deque_t *deque = deque_construct(sizeof(int), NULL, compare);
    
    puts(deque_isempty(deque) ? "true" : "false");
    puts(deque_isfull(deque) ? "true" : "false");
    printf("%lu\n", deque_size(deque));

    deque_free(deque);

    return 0;
}