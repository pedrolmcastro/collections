#include "queue.h"

#include <errno.h>
#include <stdlib.h>


typedef struct _Node {
    void *data;
    struct _Node *next;
}
_node_t;

struct _Queue {
    _node_t *head;
    _node_t *tail;
    size_t width;
    size_t size;
    void (*free_data)(void *data);
    int (*compare)(const void *first, const void *second);
};


static void _free_node(_node_t *node, void (*free_data)(void *data));


queue_t *queue_construct(size_t width, void (*free_data)(void *data), int (*compare)(const void *first, const void *second)) {
    if (width == 0 || compare == NULL) {
        errno = EINVAL;
        return NULL;
    }

    queue_t *queue = malloc(sizeof(queue_t));
    if (queue == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;

    queue->width = width;
    queue->free_data = free_data;
    queue->compare = compare;

    return queue;
}

void queue_clear(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return;
    }

    _node_t *node = queue->head;
    while (node != NULL) {
        _node_t *remove = node;
        node = node->next;

        _free_node(remove, queue->free_data);
    }

    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
}

void queue_free(queue_t *queue) {
    if (queue != NULL) {
        queue_clear(queue);
        free(queue);
    }
}


static void _free_node(_node_t *node, void (*free_data)(void *data)) {
    if (node != NULL) {
        if (free_data != NULL) {
            free_data(node->data);
        }

        free(node->data);
        free(node);
    }
}
