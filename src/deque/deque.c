#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
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


static _node_t *_construct_node(const void *data, size_t width);
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


bool deque_unshift(deque_t *deque, const void *data) {
    if (deque == NULL || data == NULL) {
        errno = EINVAL;
        return false;
    }

    if (deque_isfull(deque)) {
        errno = ENOSPC;
        return false;
    }

    _node_t *new = _construct_node(data, deque->width);
    if (new == NULL) {
        // errno set in _construct_node()
        return false;
    }

    if (deque_isempty(deque)) {
        deque->front = new;
        deque->back = new;
    }
    else {
        deque->front->previous = new;
        new->next = deque->front;
        deque->front = new;
    }
    deque->size++;

    return true;
}

bool deque_shift(deque_t *deque, void *destination) {
    if (deque_front(deque, destination) == false) {
        // errno set in deque_front()
        return false;
    }

    _node_t *remove = deque->front;
    deque->front = deque->front->next;

    if (deque->front == NULL) {
        deque->back = NULL;
    }
    else {
        deque->front->previous = NULL;
    }

    _free_node(remove, NULL);
    deque->size--;

    return true;
}

bool deque_front(deque_t *deque, void *destination) {
    if (deque == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (deque_isempty(deque)) {
        errno = EINVAL;
        return false;
    }

    memcpy(destination, deque->front->data, deque->width);

    return true;
}


bool deque_push(deque_t *deque, const void *data) {
    if (deque == NULL || data == NULL) {
        errno = EINVAL;
        return false;
    }

    if (deque_isfull(deque)) {
        errno = ENOSPC;
        return false;
    }

    _node_t *new = _construct_node(data, deque->width);
    if (new == NULL) {
        // errno set in _construct_node()
        return false;
    }

    if (deque_isempty(deque)) {
        deque->front = new;
        deque->back = new;
    }
    else {
        deque->back->next = new;
        new->previous = deque->back;
        deque->back = new;
    }
    deque->size++;

    return true;
}

bool deque_pop(deque_t *deque, void *destination) {
    if (deque_back(deque, destination) == false) {
        // errno set in deque_back()
        return false;
    }

    _node_t *remove = deque->back;
    deque->back = deque->back->previous;

    if (deque->back == NULL) {
        deque->front = NULL;
    }
    else {
        deque->back->next = NULL;
    }

    _free_node(remove, NULL);
    deque->size--;

    return true;
}

bool deque_back(deque_t *deque, void *destination) {
    if (deque == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (deque_isempty(deque)) {
        errno = EINVAL;
        return false;
    }

    memcpy(destination, deque->back->data, deque->width);

    return true;
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


static _node_t *_construct_node(const void *data, size_t width) {
    if (data == NULL || width == 0) {
        errno = EINVAL;
        return NULL;
    }

    _node_t *node = malloc(sizeof(_node_t));
    if (node == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    node->data = malloc(width);
    if (node->data == NULL) {
        free(node);
        errno = ENOMEM;
        return NULL;
    }

    memcpy(node->data, data, width);
    node->next = NULL;
    node->previous = NULL;

    return node;
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
