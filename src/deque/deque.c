#include "deque.h"


typedef struct _Node {
    void *data;
    struct _Node *next;
    struct _Node *previous;
}
_node_t;

struct Deque {
    _node_t *front;
    _node_t *back;
    size_t width;
    size_t size;
    void (*free_data)(void *data);
    int (*compare)(const void *first, const void *second);
};
