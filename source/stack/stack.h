/****************************************************************************************************
Author: Pedro Lucas de Moliner de Castro
Copyright: Open source code under MIT License
Description: Generic dynamic implementation of a stack in C
*****************************************************************************************************/

#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdbool.h>

typedef struct _Stack stack_t;

stack_t *stack_init(size_t datasize, int (*compare)(const void *first, const void *second), void (*free_data)(void *data));
void stack_clear(stack_t *stack);
void stack_free(stack_t *stack);

int stack_push(stack_t *stack, const void *data);
int stack_pop(stack_t *stack, void *destination);
int stack_peek(stack_t *stack, void *destination);

stack_t *stack_clone(stack_t *stack);
stack_t *stack_reverse(stack_t *stack);

bool stack_contains(stack_t *stack, const void *key);

size_t stack_size(stack_t *stack);

bool stack_isempty(stack_t *stack);
bool stack_isfull(stack_t *stack);

#endif //STACK_H
