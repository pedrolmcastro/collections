#ifndef DEQUE_H
#define DEQUE_H

#include <stddef.h>
#include <stdbool.h>


typedef struct _Deque deque_t;


deque_t *deque_construct(size_t width, void (*free_data)(void *data), int (*compare)(const void *first, const void *second));

void deque_free(deque_t *deque);
void deque_clear(deque_t *deque);

#endif // DEQUE_H
