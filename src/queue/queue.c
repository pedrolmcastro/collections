#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "queue.h"


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


static _node_t *_construct_node(const void *data, size_t width);
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

queue_t *queue_clone(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return NULL;
    }

    queue_t *clone = queue_construct(queue->width, queue->free_data, queue->compare);
    if (clone == NULL) {
        // errno set in queue_construct()
        return NULL;
    }

    for (_node_t *node = queue->head; node != NULL; node = node->next) {
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

    queue_t *reversed = queue_construct(queue->width, queue->free_data, queue->compare);
    if (reversed == NULL) {
        // errno set in queue_construct()
        return NULL;
    }

    for (_node_t *node = queue->head; node != NULL; node = node->next) {
        _node_t *new = _construct_node(node->data, queue->width);
        if (new == NULL) {
            queue_free(reversed);
            // errno set in _construct_node()
            return NULL;
        }

        if (queue_isempty(reversed)) {
            reversed->head = new;
            reversed->tail = new;
        }
        else {
            new->next = reversed->head;
            reversed->head = new;
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


bool queue_enqueue(queue_t *queue, const void *data) {
    if (queue == NULL || data == NULL) {
        errno = EINVAL;
        return false;
    }

    if (queue_isfull(queue)) {
        errno = ENOSPC;
        return false;
    }

    _node_t *new = _construct_node(data, queue->width);
    if (new == NULL) {
        // errno set in _construct_node()
        return false;
    }

    if (queue_isempty(queue)) {
        queue->head = new;
        queue->tail = new;
    }
    else {
        queue->tail->next = new;
        queue->tail = new;
    }
    queue->size++;

    return true;
}

bool queue_dequeue(queue_t *queue, void *destination) {
    if (queue_peek(queue, destination) == false) {
        // errno set in queue_peek()
        return false;
    }

    _node_t *remove = queue->head;
    queue->head = queue->head->next;
    
    _free_node(remove, NULL);
    queue->size--;

    return true;
}

bool queue_peek(queue_t *queue, void *destination) {
    if (queue == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (queue_isempty(queue)) {
        errno = EINVAL;
        return false;
    }

    memcpy(destination, queue->head->data, queue->width);

    return true;
}


bool queue_isempty(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return false;
    }

    return queue->head == NULL;
}

bool queue_isfull(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return false;
    }

    return queue->size == SIZE_MAX;
}

size_t queue_size(queue_t *queue) {
    if (queue == NULL) {
        errno = EINVAL;
        return 0;
    }

    return queue->size;
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
