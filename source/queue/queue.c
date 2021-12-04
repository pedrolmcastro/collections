#include "queue.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct _Node {
    void *data;
    struct _Node *next;
}
_node_t;

struct _Queue {
    _node_t *front;
    _node_t *back;
    size_t size;
    size_t width;
    size_t limit;
    void (*delete)(void *data);
    bool (*clone)(const void *source, void *destination);
};


static bool _data_clone(queue_t *queue, const void *data, void *destination);
static _node_t *_node_construct(queue_t *queue, const void *data);
static void _node_free(queue_t *queue, _node_t *node);


queue_t *queue_construct(size_t width, size_t limit, bool (*clone)(const void *source, void *destination), void (*delete)(void *data)) {
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
    queue->clone = clone;
    queue->delete = delete;

    return queue;
}

queue_t *queue_clone(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return NULL;
    }

    queue_t *clone = queue_construct(queue->width, queue->limit, queue->clone, queue->delete);
    if (clone == NULL) {
        // errno set in queue_construct()
        return NULL;
    }

    for (_node_t *node = queue->front; node != NULL; node = node->next) {
        if (queue_enqueue(clone, node->data) == false) {
            queue_free(clone);
            // errno set in queue_enqueue()
            return NULL;
        }
    }

    return clone;
}

queue_t *queue_reverse(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return NULL;
    }

    queue_t *reversed = queue_construct(queue->width, queue->limit, queue->clone, queue->delete);
    if (reversed == NULL) {
        // errno set in queue_construct()
        return NULL;
    }

    for (_node_t *node = queue->front; node != NULL; node = node->next) {
        _node_t *new = _node_construct(reversed, node->data);
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


void queue_free(queue_t *queue) {
    if (queue != NULL) {
        queue_clear(queue);
        free(queue);
    }
}

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


bool queue_enqueue(queue_t *queue, const void *data) {
    if (queue == NULL || data == NULL) {
        errno = EINVAL;
        return false;
    }

    if (queue_full(queue)) {
        errno = ENOSPC;
        return false;
    }

    _node_t *new = _node_construct(queue, data);
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

bool queue_peek(queue_t *queue, void *destination) {
    if (queue == NULL || destination == NULL || queue_empty(queue)) {
        errno = EINVAL;
        return false;
    }

    // errno set in _data_clone()
    return _data_clone(queue, queue->front->data, destination);
}


bool queue_contains(queue_t *queue, const void *key, int (*compare)(const void *data, const void *key)) {
    if (queue == NULL || key == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    for (_node_t *node = queue->front; node != NULL; node = node->next) {
        if (compare(node->data, key) == 0) {
            return true;
        }
    }

    return false;
}


size_t queue_size(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return 0;
    }

    return queue->size;
}

size_t queue_width(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return 0;
    }

    return queue->width;
}

size_t queue_limit(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return 0;
    }

    return queue->limit;
}


bool queue_empty(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return false;
    }

    return queue->front == NULL;
}

bool queue_full(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return false;
    }

    return queue->size == queue->limit;
}


static bool _data_clone(queue_t *queue, const void *data, void *destination) {
    if (queue == NULL || data == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (queue->clone != NULL) {
        // errno set in clone()
        return queue->clone(data, destination);
    }

    memcpy(destination, data, queue->width);
    return true;
}

static _node_t *_node_construct(queue_t *queue, const void *data) {
    if (queue == NULL || data == NULL) {
        errno = EINVAL;
        return NULL;
    }

    _node_t *new = malloc(sizeof(_node_t));
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    new->data = malloc(queue->width);
    if (new->data == NULL) {
        free(new);
        errno = ENOMEM;
        return NULL;
    }

    if (_data_clone(queue, data, new->data) == false) {
        free(new->data);
        free(new);
        // errno set in _data_clone()
        return NULL;
    }

    new->next = NULL;

    return new;
}

static void _node_free(queue_t *queue, _node_t *node) {
    if (queue == NULL) {
        errno = EINVAL;
        return;
    }

    if (node != NULL) {
        if (queue->delete != NULL) {
            queue->delete(node->data);
        }

        free(node->data);
        free(node);
    }
}
