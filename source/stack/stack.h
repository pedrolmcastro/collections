#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdbool.h>


typedef struct _Stack stack_t;
extern const size_t STACK_LIMIT;


stack_t *stack_construct(size_t width, size_t limit, size_t capacity, double increment, bool (*clone)(const void *source, void *destination), void (*delete)(void *data));
stack_t *stack_clone(stack_t *stack);
stack_t *stack_reverse(stack_t *stack);

void stack_free(stack_t *stack);
bool stack_clear(stack_t *stack);

bool stack_reserve(stack_t *stack, size_t size);
bool stack_trim(stack_t *stack);

bool stack_push(stack_t *stack, const void *data);
bool stack_pop(stack_t *stack, void *destination);
bool stack_peek(stack_t *stack, void *destination);

bool stack_contains(stack_t *stack, const void *key, int (*compare)(const void *data, const void *key));

size_t stack_size(stack_t *stack);
size_t stack_width(stack_t *stack);
size_t stack_limit(stack_t *stack);
size_t stack_capacity(stack_t *stack);
double stack_increment(stack_t *stack);

bool stack_empty(stack_t *stack);
bool stack_full(stack_t *stack);

#endif //STACK_H
