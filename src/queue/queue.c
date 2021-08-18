#include "queue.h"

#include <stdlib.h>


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
