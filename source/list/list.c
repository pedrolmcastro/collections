/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file list.c
 * 
 * @brief Generic doubly linked list implementation
 */


#include "list.h"

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


typedef struct _Node {
    void *value;
    struct _Node *next;
    struct _Node *previous;
}
_node_t;

struct _List {
    _node_t *front;
    _node_t *back;
    size_t size;
    size_t width;
    size_t limit;
    void (*free_)(void *value);
    bool (*copy)(const void *source, void *destination);
};


// Auxiliary functions for values
static void *_value_construct(const list_t *list, const void *value);
static bool _value_copy(const list_t *list, const void *value, void *destination);
static void _value_free(const list_t *list, void *value);

// Auxiliary functions for nodes
static _node_t *_node_construct(const list_t *list, const void *value);
static void _node_free(const list_t *list, _node_t *node);
static bool _node_remove(list_t *list, _node_t *node);
static _node_t *_node_search(const list_t *list, size_t index);

// Auxiliary functions for sorting
static _node_t *_list_mergesort(_node_t *front, int (*compare)(const void *first, const void *second));


bool list_clear(list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return false;
    }

    _node_t *node = list->front;
    
    while (node != NULL) {
        _node_t *remove = node;
        node = node->next;

        _node_free(list, remove);
    }

    list->front = NULL;
    list->back = NULL;
    list->size = 0;

    return true;
}

list_t *list_construct(size_t width, size_t limit, bool (*copy)(const void *source, void *destination), void (*free_)(void *value)) {
    if (width == 0 || limit == 0) {
        errno = EINVAL;
        return NULL;
    }

    list_t *list = malloc(sizeof(list_t));
    if (list == NULL) {
        errno = ENOMEM;
        return false;
    }

    list->front = NULL;
    list->back = NULL;
    list->size = 0;

    list->width = width;
    list->limit = limit;
    list->copy = copy;
    list->free_ = free_;

    return list;
}

bool list_contains(const list_t *list, const void *key, int (*compare)(const void *value, const void *key)) {
    // errno set in list_search()
    return list_search(list, key, compare) != SIZE_MAX;
}

list_t *list_copy(const list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return NULL;
    }

    list_t *copy = list_construct(list->width, list->limit, list->copy, list->free_);
    if (copy == NULL) {
        // errno set in list_construct()
        return NULL;
    }

    for (_node_t *node = list->front; node != NULL; node = node->next) {
        if (list_insert(copy, copy->size, node->value) == false) {
            list_free(copy);
            // errno set in list_insert()
            return NULL;
        }
    }

    return copy;
}

bool list_empty(const list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return false;
    }

    return list->front == NULL;
}

void list_free(list_t *list) {
    if (list != NULL) {
        list_clear(list);
        free(list);
    }
}

bool list_full(const list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return false;
    }

    return list->size == list->limit;
}

bool list_get(const list_t *list, size_t index, void *destination) {
    if (list == NULL || index >= list->size || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    _node_t *node = _node_search(list, index);

    // errno set in _value_copy()
    return _value_copy(list, node->value, destination);
}

bool list_insert(list_t *list, size_t index, const void *value) {
    if (list == NULL || index > list->size || value == NULL) {
        errno = EINVAL;
        return false;
    }

    if (list_full(list)) {
        errno = ENOSPC;
        return false;
    }

    _node_t *new = _node_construct(list, value);
    if (new == NULL) {
        // errno set in _node_construct()
        return false;
    }

    if (list_empty(list)) {
        list->front = new;
        list->back = new;
    }
    else if (index == 0) {
        new->next = list->front;

        list->front->previous = new;
        list->front = new;
    }
    else if (index == list->size) {
        new->previous = list->back;

        list->back->next = new;
        list->back = new;
    }
    else {
        _node_t *node = _node_search(list, index);

        new->previous = node->previous;
        new->next = node;

        node->previous->next = new;
        node->previous = new;
    }

    list->size++;

    return true;
}

size_t list_limit(const list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return 0;
    }

    return list->limit;
}

bool list_remove(list_t *list, size_t index, void *destination) {
    if (list == NULL || index >= list->size) {
        errno = EINVAL;
        return false;
    }

    _node_t *remove = _node_search(list, index);

    if (destination != NULL && _value_copy(list, remove->value, destination) == false) {
        // errno set in _value_copy()
        return false;
    }

    // errno set in _node_remove()
    return _node_remove(list, remove);
}

bool list_removeall(list_t *list, const void *remove, int (*compare)(const void *value, const void *remove)) {
    if (list == NULL || remove == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    _node_t *node = list->front;

    while (node != NULL) {
        _node_t *temporary = node;
        node = node->next;

        if (compare(temporary->value, remove) == 0) {
            _node_remove(list, temporary);
        }
    }

    return true;
}

list_t *list_reverse(const list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return NULL;
    }

    list_t *reversed = list_construct(list->width, list->limit, list->copy, list->free_);
    if (reversed == NULL) {
        // errno set in list_construct()
        return NULL;
    }

    for (_node_t *node = list->front; node != NULL; node = node->next) {
        if (list_insert(reversed, 0, node->value) == false) {
            list_free(reversed);
            // errno set in list_insert()
            return NULL;
        }
    }

    return reversed;
}

size_t list_search(const list_t *list, const void *key, int (*compare)(const void *value, const void *key)) {
    if (list == NULL || key == NULL || compare == NULL) {
        errno = EINVAL;
        return SIZE_MAX;
    }

    size_t i = 0;

    for (_node_t *node = list->front; node != NULL; node = node->next, i++) {
        if (compare(node->value, key) == 0) {
            return i;
        }
    }

    return SIZE_MAX;
}

bool list_set(list_t *list, size_t index, const void *value) {
    if (list == NULL || index >= list->size || value == NULL) {
        errno = EINVAL;
        return false;
    }

    void *new = _value_construct(list, value);
    if (new == NULL) {
        // errno set in _value_construct()
        return false;
    }

    _node_t *node = _node_search(list, index);
    _value_free(list, node->value);
    node->value = new;

    return true;
}

size_t list_size(const list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return 0;
    }

    return list->size;
}

bool list_sort(list_t *list, int (*compare)(const void *first, const void *second)) {
    if (list == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    list->front = _list_mergesort(list->front, compare);

    // Recover the list back
    for (_node_t *node = list->front; node != NULL; node = node->next) {
        list->back = node;
    }

    return true;
}

size_t list_width(const list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return 0;
    }

    return list->width;
}


static void *_value_construct(const list_t *list, const void *value) {
    if (list == NULL || value == NULL) {
        errno = EINVAL;
        return NULL;
    }

    void *new = malloc(list->width);
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (_value_copy(list, value, new) == false) {
        free(new);
        // errno set in _value_copy()
        return NULL;
    }

    return new;
}

static bool _value_copy(const list_t *list, const void *value, void *destination) {
    if (list == NULL || value == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (list->copy != NULL) {
        // errno set in copy()
        return list->copy(value, destination);
    }

    memcpy(destination, value, list->width);
    return true;
}

static void _value_free(const list_t *list, void *value) {
    if (list == NULL) {
        errno = EINVAL;
        return;
    }

    if (value != NULL) {
        if (list->free_ != NULL) {
            list->free_(value);
        }

        free(value);
    }
}


static _node_t *_node_construct(const list_t *list, const void *value) {
    if (list == NULL || value == NULL) {
        errno = EINVAL;
        return NULL;
    }

    _node_t *new = malloc(sizeof(_node_t));
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    new->value = _value_construct(list, value);
    if (new->value == NULL) {
        free(new);
        // errno set in _value_construct()
        return NULL;
    }

    new->next = NULL;
    new->previous = NULL;

    return new;
}

static void _node_free(const list_t *list, _node_t *node) {
    if (list == NULL) {
        errno = EINVAL;
        return;
    }

    if (node != NULL) {
        _value_free(list, node->value);
        free(node);
    }
}

static bool _node_remove(list_t *list, _node_t *node) {
    if (list == NULL || node == NULL) {
        errno = EINVAL;
        return false;
    }

    if (list->size == 1) {
        list->front = NULL;
        list->back = NULL;
    }
    else if (node == list->front) {
        list->front = list->front->next;
        list->front->previous = NULL;
    }
    else if (node == list->back) {
        list->back = list->back->previous;
        list->back->next = NULL;
    }
    else {
        node->previous->next = node->next;
        node->next->previous = node->previous;
    }

    _node_free(list, node);
    list->size--;

    return true;
}

static _node_t *_node_search(const list_t *list, size_t index) {
    if (list == NULL || index >= list->size) {
        errno = EINVAL;
        return NULL;
    }

    _node_t *node;

    if (index < list->size / 2) {
        node = list->front;

        for (size_t i = 0; i < index; i++) {
            node = node->next;
        }
    }
    else {
        node = list->back;

        for (size_t i = 0; i < list->size - index - 1; i++) {
            node = node->previous;
        }
    }

    return node;
}


static _node_t *_list_split(_node_t *front) {
    _node_t *fast = front;
    _node_t *slow = front;

    while (fast->next != NULL && fast->next->next != NULL) {
        fast = fast->next->next;
        slow = slow->next;
    }

    _node_t *temporary = slow->next;
    slow->next = NULL;

    return temporary;
}


static _node_t *_list_merge(_node_t *first, _node_t *second, int (*compare)(const void *first, const void *second)) {
    if (first == NULL) {
        return second;
    }

    if (second == NULL) {
        return first;
    }

    if (compare(first->value, second->value) < 0) {
        first->next = _list_merge(first->next, second, compare);
        first->next->previous = first;
        first->previous = NULL;

        return first;
    }
    else {
        second->next = _list_merge(first, second->next, compare);
        second->next->previous = second;
        second->previous = NULL;

        return second;
    }
}

static _node_t *_list_mergesort(_node_t *front, int (*compare)(const void *first, const void *second)) {
    if (front == NULL || front->next == NULL) {
        return front;
    }

    _node_t *second = _list_split(front);

    front = _list_mergesort(front, compare);
    second = _list_mergesort(second, compare);

    return _list_merge(front, second, compare);
}
