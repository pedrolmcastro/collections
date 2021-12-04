#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdbool.h>


typedef struct _Queue queue_t;


queue_t *queue_construct(size_t width, size_t limit, bool (*copy)(const void *source, void *destination), void (*free_)(void *value));
queue_t *queue_copy(queue_t *queue);
queue_t *queue_reverse(queue_t *queue);

void queue_free(queue_t *queue);
bool queue_clear(queue_t *queue);

bool queue_enqueue(queue_t *queue, const void *value);
bool queue_dequeue(queue_t *queue, void *destination);
bool queue_peek(queue_t *queue, void *destination);

bool queue_contains(queue_t *queue, const void *key, int (*compare)(const void *value, const void *key));

size_t queue_size(queue_t *queue);
size_t queue_width(queue_t *queue);
size_t queue_limit(queue_t *queue);

bool queue_empty(queue_t *queue);
bool queue_full(queue_t *queue);

#endif // QUEUE_H
