#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdbool.h>


typedef struct _Queue queue_t;


queue_t *queue_construct(size_t width, void (*free_data)(void *data), int (*compare)(const void *first, const void *second));
queue_t *queue_clone(queue_t *queue);
queue_t *queue_reverse(queue_t *queue);

void queue_free(queue_t *queue);
void queue_clear(queue_t *queue);

bool queue_enqueue(queue_t *queue, const void *data);
bool queue_dequeue(queue_t *queue, void *destination);
bool queue_peek(queue_t *queue, void *destination);

bool queue_contains(queue_t *queue, const void *key);

bool queue_isempty(queue_t *queue);
bool queue_isfull(queue_t *queue);
size_t queue_size(queue_t *queue);

#endif // QUEUE_H
