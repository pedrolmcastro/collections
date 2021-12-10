/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file queue.c
 * 
 * @brief Generic queue implementation using singly linked list
 */


#include "queue.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct _Node {
    void *value;
    struct _Node *next;
}
_node_t;

struct _Queue {
    _node_t *front;
    _node_t *back;
    size_t size;
    size_t width;
    size_t limit;
    void (*free_)(void *value);
    bool (*copy)(const void *source, void *destination);
};


// Auxiliary functions for values
static bool _value_copy(const queue_t *queue, const void *value, void *destination);

// Auxiliary functions for nodes
static _node_t *_node_construct(const queue_t *queue, const void *value);
static void _node_free(const queue_t *queue, _node_t *node);


bool queue_clear(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return false;
    }

    _node_t *node = queue->front;
    
    while (node != NULL) {
        _node_t *remove = node;
        node = node->next;

        _node_free(queue, remove);
    }

    queue->front = NULL;
    queue->back = NULL;
    queue->size = 0;

    return true;
}

queue_t *queue_construct(size_t width, size_t limit, bool (*copy)(const void *source, void *destination), void (*free_)(void *value)) {
    if (width == 0 || limit == 0) {
        errno = EINVAL;
        return NULL;
    }

    queue_t *queue = malloc(sizeof(queue_t));
    if (queue == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    queue->front = NULL;
    queue->back = NULL;
    queue->size = 0;

    queue->width = width;
    queue->limit = limit;
    queue->copy = copy;
    queue->free_ = free_;

    return queue;
}

bool queue_contains(const queue_t *queue, const void *key, int (*compare)(const void *value, const void *key)) {
    if (queue == NULL || key == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    for (_node_t *node = queue->front; node != NULL; node = node->next) {
        if (compare(node->value, key) == 0) {
            return true;
        }
    }

    return false;
}

queue_t *queue_copy(const queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return NULL;
    }

    queue_t *copy = queue_construct(queue->width, queue->limit, queue->copy, queue->free_);
    if (copy == NULL) {
        // errno set in queue_construct()
        return NULL;
    }

    for (_node_t *node = queue->front; node != NULL; node = node->next) {
        if (queue_enqueue(copy, node->value) == false) {
            queue_free(copy);
            // errno set in queue_enqueue()
            return NULL;
        }
    }

    return copy;
}

bool queue_dequeue(queue_t *queue, void *destination) {
    if (queue == NULL || queue_empty(queue)) {
        errno = EINVAL;
        return false;
    }

    if (destination != NULL && queue_peek(queue, destination) == false) {
        // errno set in queue_peek()
        return false;
    }

    _node_t *remove = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->back = NULL;
    }

    _node_free(queue, remove);
    queue->size--;

    return true;
}

bool queue_empty(const queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return false;
    }

    return queue->front == NULL;
}

bool queue_enqueue(queue_t *queue, const void *value) {
    if (queue == NULL || value == NULL) {
        errno = EINVAL;
        return false;
    }

    if (queue_full(queue)) {
        errno = ENOSPC;
        return false;
    }

    _node_t *new = _node_construct(queue, value);
    if (new == NULL) {
        // errno set in _node_construct()
        return false;
    }

    if (queue_empty(queue)) {
        queue->front = new;
        queue->back = new;
    }
    else {
        queue->back->next = new;
        queue->back = new;
    }

    queue->size++;

    return true;
}

void queue_free(queue_t *queue) {
    if (queue != NULL) {
        queue_clear(queue);
        free(queue);
    }
}

bool queue_full(const queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return false;
    }

    return queue->size == queue->limit;
}

size_t queue_limit(const queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return 0;
    }

    return queue->limit;
}

bool queue_peek(const queue_t *queue, void *destination) {
    if (queue == NULL || destination == NULL || queue_empty(queue)) {
        errno = EINVAL;
        return false;
    }

    // errno set in _value_copy()
    return _value_copy(queue, queue->front->value, destination);
}

queue_t *queue_reverse(const queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return NULL;
    }

    queue_t *reversed = queue_construct(queue->width, queue->limit, queue->copy, queue->free_);
    if (reversed == NULL) {
        // errno set in queue_construct()
        return NULL;
    }

    for (_node_t *node = queue->front; node != NULL; node = node->next) {
        _node_t *new = _node_construct(reversed, node->value);
        if (new == NULL) {
            queue_free(reversed);
            // errno set in _node_construct()
            return NULL;
        }

        if (queue_empty(reversed)) {
            reversed->front = new;
            reversed->back = new;
        }
        else {
            new->next = reversed->front;
            reversed->front = new;
        }

        reversed->size++;
    }

    return reversed;
}

size_t queue_size(const queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return 0;
    }

    return queue->size;
}

size_t queue_width(const queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return 0;
    }

    return queue->width;
}


static bool _value_copy(const queue_t *queue, const void *value, void *destination) {
    if (queue == NULL || value == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (queue->copy != NULL) {
        // errno set in copy()
        return queue->copy(value, destination);
    }

    memcpy(destination, value, queue->width);
    return true;
}


static _node_t *_node_construct(const queue_t *queue, const void *value) {
    if (queue == NULL || value == NULL) {
        errno = EINVAL;
        return NULL;
    }

    _node_t *new = malloc(sizeof(_node_t));
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    new->value = malloc(queue->width);
    if (new->value == NULL) {
        free(new);
        errno = ENOMEM;
        return NULL;
    }

    if (_value_copy(queue, value, new->value) == false) {
        free(new->value);
        free(new);
        // errno set in _value_copy()
        return NULL;
    }

    new->next = NULL;

    return new;
}

static void _node_free(const queue_t *queue, _node_t *node) {
    if (queue == NULL) {
        errno = EINVAL;
        return;
    }

    if (node != NULL) {
        if (queue->free_ != NULL) {
            queue->free_(node->value);
        }

        free(node->value);
        free(node);
    }
}
