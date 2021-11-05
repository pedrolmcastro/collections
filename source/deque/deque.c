#include <errno.h>
#include <stdlib.h>
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
    size_t size;
    size_t width;
    size_t limit;
    void (*delete)(void *data);
    bool (*clone)(const void *source, void *destination);
};


static bool _data_clone(deque_t *deque, const void *data, void *destination);
static _node_t *_node_construct(deque_t *deque, const void *data);
static void _node_free(deque_t *deque, _node_t *node);


deque_t *deque_construct(size_t width, size_t limit, bool (*clone)(const void *source, void *destination), void (*delete)(void *data)) {
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
    deque->clone = clone;
    deque->delete = delete;

    return deque;
}

deque_t *deque_clone(deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return NULL;
    }

    deque_t *clone = deque_construct(deque->width, deque->limit, deque->clone, deque->delete);
    if (clone == NULL) {
        // errno set in deque_construct()
        return NULL;
    }

    for (_node_t *node = deque->front; node != NULL; node = node->next) {
        if (deque_push(clone, node->data) == false) {
            deque_free(deque);
            // errno set in deque_push()
            return NULL;
        }
    }

    return clone;
}


void deque_free(deque_t *deque) {
    if (deque != NULL) {
        deque_clear(deque);
        free(deque);
    }
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


bool deque_unshift(deque_t *deque, const void *data) {
    if (deque == NULL || data == NULL) {
        errno = EINVAL;
        return false;
    }

    if (deque_full(deque)) {
        errno = ENOSPC;
        return false;
    }

    _node_t *new = _node_construct(deque, data);
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

bool deque_front(deque_t *deque, void *destination) {
    if (deque == NULL || destination == NULL || deque_empty(deque)) {
        errno = EINVAL;
        return false;
    }

    // errno set in _data_clone()
    return _data_clone(deque, deque->front->data, destination);
}


bool deque_push(deque_t *deque, const void *data) {
    if (deque == NULL || data == NULL) {
        errno = EINVAL;
        return false;
    }

    if (deque_full(deque)) {
        errno = ENOSPC;
        return false;
    }

    _node_t *new = _node_construct(deque, data);
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

bool deque_back(deque_t *deque, void *destination) {
    if (deque == NULL || destination == NULL || deque_empty(deque)) {
        errno = EINVAL;
        return false;
    }

    // errno set in _data_clone()
    return _data_clone(deque, deque->back->data, destination);
}


bool deque_contains(deque_t *deque, const void *key, int (*compare)(const void *data, const void *key)) {
    if (deque == NULL || key == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    for (_node_t *node = deque->front; node != NULL; node = node->next) {
        if (compare(node->data, key) == 0) {
            return true;
        }
    }

    return false;
}


size_t deque_size(deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return 0;
    }

    return deque->size;
}

size_t deque_width(deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return 0;
    }

    return deque->width;
}

size_t deque_limit(deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return 0;
    }

    return deque->limit;
}


bool deque_empty(deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return false;
    }

    return deque->front == NULL;
}

bool deque_full(deque_t *deque) {
    if (deque == NULL) {
        errno = EINVAL;
        return false;
    }

    return deque->size == deque->limit;
}


static bool _data_clone(deque_t *deque, const void *data, void *destination) {
    if (deque == NULL || data == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (deque->clone != NULL) {
        // errno set in clone()
        return deque->clone(data, destination);
    }

    memcpy(destination, data, deque->width);
    return true;
}

static _node_t *_node_construct(deque_t *deque, const void *data) {
    if (deque == NULL || data == NULL) {
        errno = EINVAL;
        return NULL;
    }

    _node_t *new = malloc(sizeof(_node_t));
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    new->data = malloc(deque->width);
    if (new->data == NULL) {
        free(new);
        errno = ENOMEM;
        return NULL;
    }

    if (_data_clone(deque, data, new->data) == false) {
        free(new->data);
        free(new);
        // errno set in _data_clone()
        return NULL;
    }

    new->next = NULL;
    new->previous = NULL;

    return new;
}

static void _node_free(deque_t *deque, _node_t *node) {
    if (deque == NULL) {
        errno = EINVAL;
        return;
    }

    if (node != NULL) {
        if (deque->delete != NULL) {
            deque->delete(node->data);
        }

        free(node->data);
        free(node);
    }
}
