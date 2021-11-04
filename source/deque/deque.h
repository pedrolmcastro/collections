#ifndef DEQUE_H
#define DEQUE_H

#include <stddef.h>
#include <stdbool.h>


typedef struct _Deque deque_t;


deque_t *deque_construct(size_t width, size_t limit, bool (*clone)(const void *source, void *destination), void (*delete)(void *data));
deque_t *deque_clone(deque_t *deque);

void deque_free(deque_t *deque);
bool deque_clear(deque_t *deque);

bool deque_unshift(deque_t *deque, const void *data);
bool deque_shift(deque_t *deque, void *destination);
bool deque_front(deque_t *deque, void *destination);

bool deque_push(deque_t *deque, const void *data);
bool deque_pop(deque_t *deque, void *destination);
bool deque_back(deque_t *deque, void *destination);

bool deque_contains(deque_t *deque, const void *key, int (*compare)(const void *data, const void *key));

size_t deque_size(deque_t *deque);
size_t deque_width(deque_t *deque);
size_t deque_limit(deque_t *deque);

bool deque_empty(deque_t *deque);
bool deque_full(deque_t *deque);

#endif // DEQUE_H
