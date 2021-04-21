/****************************************************************************************************
Author: Pedro Lucas de Moliner de Castro
Copyright: Open source code under MIT License
Description: Generic dynamic implementation of a stack in C
*****************************************************************************************************/

#include "stack.h"

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifndef RETURN_H
    #define RETURN_H
    #define RETURN_SUCCESS 0
    #define RETURN_FAILURE -1
#endif

typedef struct _Node {
    void *data;
    struct _Node *next;
} _node_t;

struct _Stack {
    _node_t *top;
    size_t size;
    size_t datasize;
    void (*free_data)(void *data);
    int (*compare)(const void *first, const void *second);
};


static _node_t *_init_node(const void *data, size_t datasize);
static void _free_node(_node_t *remove, void (*free_data)(void *data));


stack_t *stack_init(size_t datasize, int (*compare)(const void *first, const void *second), void (*free_data)(void *data)) {
    if (datasize == 0 || compare == NULL) {
        errno = EINVAL;
        return NULL;
    }

    stack_t *stack = malloc(sizeof(stack_t));
    if (stack == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    stack->top = NULL;
    stack->size = 0;
    stack->datasize = datasize;
    stack->free_data = free_data;
    stack->compare = compare;

    return stack;
}

void stack_clear(stack_t *stack) {
    if (stack == NULL) {
        errno = EFAULT;
        return;
    }

    _node_t *node = stack->top;
    while (node != NULL) {
        _node_t *remove = node;
        node = node->next;
        _free_node(remove, stack->free_data);
    }

    stack->top = NULL;
    stack->size = 0;
}

void stack_free(stack_t *stack) {
    if (stack != NULL) {
        _node_t *node = stack->top;
        while (node != NULL) {
            _node_t *remove = node;
            node = node->next;
            _free_node(remove, stack->free_data);
        }

        free(stack);
    }
}


int stack_push(stack_t *stack, const void *data) {
    if (stack == NULL || data == NULL) {
        errno = EFAULT;
        return RETURN_FAILURE;
    }

    if (stack_isfull(stack)) {
        errno = ENOMEM;
        return RETURN_FAILURE;
    }

    _node_t *newnode = _init_node(data, stack->datasize);
    if (newnode == NULL) {
        errno = ENOMEM;
        return RETURN_FAILURE;
    }

    newnode->next = stack->top;
    stack->top = newnode;
    stack->size++;

    return RETURN_SUCCESS;
}

int stack_pop(stack_t *stack, void *destination) {
    if (stack == NULL) {
        errno = EFAULT;
        return RETURN_FAILURE;
    }

    if (stack_isempty(stack)) {
        errno = EINVAL;
        return RETURN_FAILURE;
    }

    _node_t *remove = stack->top;
    stack->top = stack->top->next;
    stack->size--;

    if (destination != NULL) {
        memcpy(destination, remove->data, stack->datasize);
        _free_node(remove, NULL);
    }
    else {
        _free_node(remove, stack->free_data);
    }

    return RETURN_SUCCESS;
}

int stack_peek(stack_t *stack, void *destination) {
    if (stack == NULL || destination == NULL) {
        errno = EFAULT;
        return RETURN_FAILURE;
    }

    if (stack_isempty(stack)) {
        errno = EINVAL;
        return RETURN_FAILURE;
    }

    memcpy(destination, stack->top->data, stack->datasize);

    return RETURN_SUCCESS;
}


stack_t *stack_clone(stack_t *stack) {
    if (stack == NULL) {
        errno = EFAULT;
        return NULL;
    }

    stack_t *clone = stack_init(stack->datasize, stack->compare, stack->free_data);
    if (clone == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    clone->size = stack->size;

    for (_node_t *prev = NULL, *node = stack->top; node != NULL; node = node->next) {
        _node_t *newnode = _init_node(node->data, clone->datasize);
        if (newnode == NULL) {
            stack_free(clone);
            errno = ENOMEM;
            return NULL;
        }

        if (node == stack->top)
            clone->top = newnode;
        else
            prev->next = newnode;
        prev = newnode;
    }

    return clone;
}

stack_t *stack_reverse(stack_t *stack) {
    if (stack == NULL) {
        errno = EFAULT;
        return NULL;
    }

    stack_t *reversed = stack_init(stack->datasize, stack->compare, stack->free_data);
    if (reversed == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    reversed->size = stack->size;

    for (_node_t *node = stack->top; node != NULL; node = node->next) {
        _node_t *newnode = _init_node(node->data, reversed->datasize);
        if (newnode == NULL) {
            stack_free(reversed);
            errno = ENOMEM;
            return NULL;
        }

        newnode->next = reversed->top;
        reversed->top = newnode;
    }

    return reversed;
}


bool stack_contains(stack_t *stack, const void *key) {
    if (stack == NULL || key == NULL) {
        errno = EFAULT;
        return false;
    }

    for (_node_t *node = stack->top; node != NULL; node = node->next) {
        if (stack->compare(node->data, key) == 0)
            return true;
    }

    return false;
}


size_t stack_size(stack_t *stack) {
    if (stack == NULL) {
        errno = EFAULT;
        return 0;
    }

    return stack->size;
}


bool stack_isempty(stack_t *stack) {
    if (stack == NULL) {
        errno = EFAULT;
        return false;
    }

    return stack->top == NULL;
}

bool stack_isfull(stack_t *stack) {
    if (stack == NULL) {
        errno = EFAULT;
        return false;
    }

    return stack->size == SIZE_MAX;
}


static _node_t *_init_node(const void *data, size_t datasize) {
    _node_t *node = malloc(sizeof(_node_t));
    if (node == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    node->data = malloc(datasize);
    if (node->data == NULL) {
        free(node);
        errno = ENOMEM;
        return NULL;
    }
    memcpy(node->data, data, datasize);

    node->next = NULL;

    return node;
}

static void _free_node(_node_t *node, void (*free_data)(void *data)) {
    if (node != NULL) {
        if (free_data != NULL)
            free_data(node->data);
        free(node->data);
        free(node);
    }
}
