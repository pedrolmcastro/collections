#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "deque.h"


typedef struct _Node {
    void *data;
    struct _Node *next;
    struct _Node *previous;
}
_node_t;

struct _Deque {
    _node_t *front;
    _node_t *back;
    size_t width;
    size_t size;
    void (*free_data)(void *data);
    int (*compare)(const void *first, const void *second);
};


static void _free_node(_node_t *node, void (*free_data)(void *data));


deque_t *deque_construct(size_t width, void (*free_data)(void *data), int (*compare)(const void *first, const void *second)) {
    if (width == 0 || compare == NULL) {
        errno = EINVAL;
        return NULL;
    }

    deque_t *deque = malloc(sizeof(deque_t));
    if (deque == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    deque->front = NULL;
    deque->back = NULL;
    deque->size = 0;

    deque->width = width;
    deque->free_data = free_data;
    deque->compare = compare;

    return deque;
}


void deque_free(deque_t *deque) {
    if (deque != NULL) {
        deque_clear(deque);
        free(deque);
    }
}

void deque_clear(deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return;
    }

    _node_t *node = deque->front;
    while (node != NULL) {
        _node_t *remove = node;
        node = node->next;

        _free_node(remove, deque->free_data);
    }

    deque->front = NULL;
    deque->back = NULL;
    deque->size = 0;
}


bool deque_isempty(deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return false;
    }

    return deque->front == NULL;
}

bool deque_isfull(deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return false;
    }

    return deque->size == SIZE_MAX;
}

size_t deque_size(deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return 0;
    }

    return deque->size;
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
