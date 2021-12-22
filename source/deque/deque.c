/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file deque.c
 * 
 * @brief Generic deque implementation using doubly linked list
 */


#include "deque.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct _Node {
    void *value;
    struct _Node *next;
    struct _Node *previous;
}
_node_t;

struct _Deque {
    _node_t *front;
    _node_t *back;
    size_t size;
    size_t width;
    size_t limit;
    void (*free_)(void *value);
    bool (*copy)(const void *source, void *destination);
};


// Auxiliary functions for values
static bool _value_copy(const deque_t *deque, const void *value, void *destination);

// Auxiliary functions for nodes
static _node_t *_node_construct(const deque_t *deque, const void *value);
static void _node_free(const deque_t *deque, _node_t *node);


bool deque_back(const deque_t *deque, void *destination) {
    if (deque == NULL || destination == NULL || deque_empty(deque)) {
        errno = EINVAL;
        return false;
    }

    // errno set in _value_copy()
    return _value_copy(deque, deque->back->value, destination);
}

bool deque_clear(deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return false;
    }

    _node_t *node = deque->front;
    
    while (node != NULL) {
        _node_t *remove = node;
        node = node->next;

        _node_free(deque, remove);
    }

    deque->front = NULL;
    deque->back = NULL;
    deque->size = 0;

    return true;
}

deque_t *deque_construct(size_t width, size_t limit, bool (*copy)(const void *source, void *destination), void (*free_)(void *value)) {
    if (width == 0 || limit == 0) {
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
    deque->limit = limit;
    deque->copy = copy;
    deque->free_ = free_;

    return deque;
}

bool deque_contains(const deque_t *deque, const void *key, int (*compare)(const void *value, const void *key)) {
    if (deque == NULL || key == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    for (_node_t *node = deque->front; node != NULL; node = node->next) {
        if (compare(node->value, key) == 0) {
            return true;
        }
    }

    return false;
}

deque_t *deque_copy(const deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return NULL;
    }

    deque_t *copy = deque_construct(deque->width, deque->limit, deque->copy, deque->free_);
    if (copy == NULL) {
        // errno set in deque_construct()
        return NULL;
    }

    for (_node_t *node = deque->front; node != NULL; node = node->next) {
        if (deque_push(copy, node->value) == false) {
            deque_free(copy);
            // errno set in deque_push()
            return NULL;
        }
    }

    return copy;
}

bool deque_empty(const deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return false;
    }

    return deque->front == NULL;
}

void deque_free(deque_t *deque) {
    if (deque != NULL) {
        deque_clear(deque);
        free(deque);
    }
}

bool deque_front(const deque_t *deque, void *destination) {
    if (deque == NULL || destination == NULL || deque_empty(deque)) {
        errno = EINVAL;
        return false;
    }

    // errno set in _value_copy()
    return _value_copy(deque, deque->front->value, destination);
}

bool deque_full(const deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return false;
    }

    return deque->size == deque->limit;
}

size_t deque_limit(const deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return 0;
    }

    return deque->limit;
}

bool deque_pop(deque_t *deque, void *destination) {
    if (deque == NULL || deque_empty(deque)) {
        errno = EINVAL;
        return false;
    }

    if (destination != NULL && deque_back(deque, destination) == false) {
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

    _node_free(deque, remove);
    deque->size--;

    return true;
}

bool deque_push(deque_t *deque, const void *value) {
    if (deque == NULL || value == NULL) {
        errno = EINVAL;
        return false;
    }

    if (deque_full(deque)) {
        errno = ENOSPC;
        return false;
    }

    _node_t *new = _node_construct(deque, value);
    if (new == NULL) {
        // errno set in _node_construct()
        return false;
    }

    if (deque_empty(deque)) {
        deque->front = new;
        deque->back = new;
    }
    else {
        new->previous = deque->back;

        deque->back->next = new;
        deque->back = new;
    }

    deque->size++;

    return true;
}

bool deque_shift(deque_t *deque, void *destination) {
    if (deque == NULL || deque_empty(deque)) {
        errno = EINVAL;
        return false;
    }

    if (destination != NULL && deque_front(deque, destination) == false) {
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

    _node_free(deque, remove);
    deque->size--;

    return true;
}

size_t deque_size(const deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return 0;
    }

    return deque->size;
}

bool deque_unshift(deque_t *deque, const void *value) {
    if (deque == NULL || value == NULL) {
        errno = EINVAL;
        return false;
    }

    if (deque_full(deque)) {
        errno = ENOSPC;
        return false;
    }

    _node_t *new = _node_construct(deque, value);
    if (new == NULL) {
        // errno set in _node_construct()
        return false;
    }

    if (deque_empty(deque)) {
        deque->front = new;
        deque->back = new;
    }
    else {
        new->next = deque->front;

        deque->front->previous = new;
        deque->front = new;
    }

    deque->size++;

    return true;
}

size_t deque_width(const deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return 0;
    }

    return deque->width;
}


static bool _value_copy(const deque_t *deque, const void *value, void *destination) {
    if (deque == NULL || value == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (deque->copy != NULL) {
        // errno set in copy()
        return deque->copy(value, destination);
    }

    memcpy(destination, value, deque->width);
    return true;
}


static _node_t *_node_construct(const deque_t *deque, const void *value) {
    if (deque == NULL || value == NULL) {
        errno = EINVAL;
        return NULL;
    }

    _node_t *new = malloc(sizeof(_node_t));
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    new->value = malloc(deque->width);
    if (new->value == NULL) {
        free(new);
        errno = ENOMEM;
        return NULL;
    }

    if (_value_copy(deque, value, new->value) == false) {
        free(new->value);
        free(new);
        // errno set in _value_copy()
        return NULL;
    }

    new->next = NULL;
    new->previous = NULL;

    return new;
}

static void _node_free(const deque_t *deque, _node_t *node) {
    if (deque == NULL) {
        errno = EINVAL;
        return;
    }

    if (node != NULL) {
        if (deque->free_ != NULL) {
            deque->free_(node->value);
        }

        free(node->value);
        free(node);
    }
}
