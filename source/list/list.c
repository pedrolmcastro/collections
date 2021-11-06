#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "list.h"


typedef struct _Node {
    void *data;
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
    void (*delete)(void *data);
    bool (*clone)(const void *source, void *destination);
};


static bool _data_clone(list_t *list, const void *data, void *destination);
static void *_data_construct(list_t *list, const void *data);
static void _data_free(list_t *list, void *data);

static _node_t *_node_construct(list_t *list, const void *data);
static _node_t *_node_search(list_t *list, size_t index);
static bool _node_remove(list_t *list, _node_t *node);
static void _node_free(list_t *list, _node_t *node);

// Auxiliary function for list_sort()
static _node_t *_list_mergesort(_node_t *front, bool reverse, int (*compare)(const void *first, const void *second));


list_t *list_construct(size_t width, size_t limit, bool (*clone)(const void *source, void *destination), void (*delete)(void *data)) {
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
    list->clone = clone;
    list->delete = delete;

    return list;
}

list_t *list_clone(list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return NULL;
    }

    list_t *clone = list_construct(list->width, list->limit, list->clone, list->delete);
    if (clone == NULL) {
        // errno set in list_construct()
        return NULL;
    }

    for (_node_t *node = list->front; node != NULL; node = node->next) {
        if (list_insert(clone, clone->size, node->data) == false) {
            list_free(clone);
            // errno set in list_insert()
            return NULL;
        }
    }

    return clone;
}

list_t *list_reverse(list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return NULL;
    }

    list_t *reversed = list_construct(list->width, list->limit, list->clone, list->delete);
    if (reversed == NULL) {
        // errno set in list_construct()
        return NULL;
    }

    for (_node_t *node = list->front; node != NULL; node = node->next) {
        if (list_insert(reversed, 0, node->data) == false) {
            list_free(reversed);
            // errno set in list_insert()
            return NULL;
        }
    }

    return reversed;
}


void list_free(list_t *list) {
    if (list != NULL) {
        list_clear(list);
        free(list);
    }
}

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


bool list_insert(list_t *list, size_t index, const void *data) {
    if (list == NULL || index > list->size || data == NULL) {
        errno = EINVAL;
        return false;
    }

    if (list_full(list)) {
        errno = ENOSPC;
        return false;
    }

    _node_t *new = _node_construct(list, data);
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

bool list_remove(list_t *list, size_t index, void *destination) {
    if (list == NULL || index >= list->size) {
        errno = EINVAL;
        return false;
    }

    _node_t *remove = _node_search(list, index);

    if (destination != NULL && _data_clone(list, remove->data, destination) == false) {
        // errno set in _data_clone()
        return false;
    }

    // errno set in _node_remove()
    return _node_remove(list, remove);
}

bool list_removeall(list_t *list, const void *remove, int (*compare)(const void *data, const void *remove)) {
    if (list == NULL || remove == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    _node_t *node = list->front;

    while (node != NULL) {
        _node_t *temporary = node;
        node = node->next;

        if (compare(temporary->data, remove) == 0) {
            if (_node_remove(list, temporary) == false) {
                // errno set in _node_remove()
                return false;
            }
        }
    }

    return true;
}


bool list_get(list_t *list, size_t index, void *destination) {
    if (list == NULL || index >= list->size || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    _node_t *node = _node_search(list, index);

    // errno set in _data_clone()
    return _data_clone(list, node->data, destination);
}

bool list_set(list_t *list, size_t index, const void *data) {
    if (list == NULL || index >= list->size || data == NULL) {
        errno = EINVAL;
        return false;
    }

    void *new = _data_construct(list, data);
    if (new == NULL) {
        // errno set in _data_construct()
        return false;
    }

    _node_t *node = _node_search(list, index);
    _data_free(list, node->data);
    node->data = new;

    return true;
}

bool list_sort(list_t *list, bool reverse, int (*compare)(const void *first, const void *second)) {
    if (list == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    list->front = _list_mergesort(list->front, reverse, compare);

    for (_node_t *node = list->front; node != NULL; node = node->next) {
        list->back = node;
    }

    return true;
}


bool list_contains(list_t *list, const void *key, int (*compare)(const void *data, const void *key)) {
    // errno set in list_search()
    return list_search(list, key, compare) != SIZE_MAX;
}

size_t list_search(list_t *list, const void *key, int (*compare)(const void *data, const void *key)) {
    if (list == NULL || key == NULL || compare == NULL) {
        errno = EINVAL;
        return SIZE_MAX;
    }

    size_t i = 0;

    for (_node_t *node = list->front; node != NULL; node = node->next, i++) {
        if (compare(node->data, key) == 0) {
            return i;
        }
    }

    return SIZE_MAX;
}


size_t list_size(list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return 0;
    }

    return list->size;
}

size_t list_width(list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return 0;
    }

    return list->width;
}

size_t list_limit(list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return 0;
    }

    return list->limit;
}


bool list_empty(list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return false;
    }

    return list->front == NULL;
}

bool list_full(list_t *list) {
    if (list == NULL) {
        errno = EINVAL;
        return false;
    }

    return list->size == list->limit;
}


static bool _data_clone(list_t *list, const void *data, void *destination) {
    if (list == NULL || data == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (list->clone != NULL) {
        // errno set in clone()
        return list->clone(data, destination);
    }

    memcpy(destination, data, list->width);
    return true;
}

static void *_data_construct(list_t *list, const void *data) {
    if (list == NULL || data == NULL) {
        errno = EINVAL;
        return NULL;
    }

    void *new = malloc(list->width);
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (_data_clone(list, data, new) == false) {
        free(new);
        // errno set in _data_clone()
        return NULL;
    }

    return new;
}

static void _data_free(list_t *list, void *data) {
    if (list == NULL) {
        errno = EINVAL;
        return;
    }

    if (data != NULL) {
        if (list->delete != NULL) {
            list->delete(data);
        }

        free(data);
    }
}


static _node_t *_node_construct(list_t *list, const void *data) {
    if (list == NULL || data == NULL) {
        errno = EINVAL;
        return NULL;
    }

    _node_t *new = malloc(sizeof(_node_t));
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    new->data = _data_construct(list, data);
    if (new->data == NULL) {
        free(new);
        // errno set in _data_construct()
        return NULL;
    }

    new->next = NULL;
    new->previous = NULL;

    return new;
}

static _node_t *_node_search(list_t *list, size_t index) {
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

static void _node_free(list_t *list, _node_t *node) {
    if (list == NULL) {
        errno = EINVAL;
        return;
    }

    if (node != NULL) {
        _data_free(list, node->data);
        free(node);
    }
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

static _node_t *_list_merge(_node_t *first, _node_t *second, bool reverse, int (*compare)(const void *first, const void *second)) {
    if (first == NULL) {
        return second;
    }

    if (second == NULL) {
        return first;
    }

    _node_t *smaller;

    if ((reverse ? -1 : 1) * compare(first->data, second->data) < 0) {
        first->next = _list_merge(first->next, second, reverse, compare);
        first->next->previous = first;
        first->previous = NULL;

        smaller = first;
    }
    else {
        second->next = _list_merge(first, second->next, reverse, compare);
        second->next->previous = second;
        second->previous = NULL;

        smaller = second;
    }

    return smaller;
}

static _node_t *_list_mergesort(_node_t *front, bool reverse, int (*compare)(const void *first, const void *second)) {
    if (front == NULL || front->next == NULL) {
        return front;
    }

    _node_t *second = _list_split(front);

    front = _list_mergesort(front, reverse, compare);
    second = _list_mergesort(second, reverse, compare);

    return _list_merge(front, second, reverse, compare);
}
