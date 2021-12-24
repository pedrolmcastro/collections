/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file stack.c
 * 
 * @brief Generic stack implementation using a vector
 */


#include "stack.h"

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


struct _Stack {
    size_t size;
    size_t width;
    size_t limit;
    size_t capacity;
    double growth;
    void **values;
    void (*free_)(void *value);
    bool (*copy)(const void *source, void *destination);
};


const size_t STACK_LIMIT = SIZE_MAX / (2 * sizeof(void *)) - 1;


// Auxiliary functions for values
static void *_value_construct(const stack_t *stack, const void *value);
static bool _value_copy(const stack_t *stack, const void *value, void *destination);
static void _value_free(const stack_t *stack, void *value);


size_t stack_capacity(const stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return 0;
    }

    return stack->capacity;
}

bool stack_clear(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < stack->size; i++) {
        _value_free(stack, stack->values[i]);
    }

    stack->size = 0;

    return true;
}

stack_t *stack_construct(size_t width, size_t limit, size_t capacity, double growth, bool (*copy)(const void *source, void *destination), void (*free_)(void *value)) {
    if (width == 0 || limit == 0 || limit > STACK_LIMIT || capacity > limit || growth < 2) {
        errno = EINVAL;
        return NULL;
    }

    stack_t *stack = malloc(sizeof(stack_t));
    if (stack == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (capacity > 0) {
        stack->values = malloc(capacity * sizeof(void *));
        if (stack->values == NULL) {
            stack_free(stack);
            errno = ENOMEM;
            return NULL;
        }
    }
    else {
        stack->values = NULL;
    }

    stack->size = 0;

    stack->width = width;
    stack->limit = limit;
    stack->capacity = capacity;
    stack->growth = growth;
    stack->copy = copy;
    stack->free_ = free_;

    return stack;
}

bool stack_contains(const stack_t *stack, const void *key, int (*compare)(const void *value, const void *key)) {
    if (stack == NULL || key == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < stack->size; i++) {
        if (compare(stack->values[i], key) == 0) {
            return true;
        }
    }

    return false;
}

stack_t *stack_copy(const stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return NULL;
    }

    stack_t *copy = stack_construct(stack->width, stack->limit, stack->capacity, stack->growth, stack->copy, stack->free_);
    if (copy == NULL) {
        // errno set in stack_construct()
        return NULL;
    }

    for (size_t i = 0; i < stack->size; i++) {
        if (stack_push(copy, stack->values[i]) == false) {
            stack_free(copy);
            // errno set in stack_push()
            return NULL;
        }
    }
    
    return copy;
}

bool stack_empty(const stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return false;
    }

    return stack->size == 0;
}

void stack_free(stack_t *stack) {
    if (stack != NULL) {
        if (stack->values != NULL) {
            stack_clear(stack);
            stack_trim(stack);
        }

        free(stack);
    }
}

bool stack_full(const stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return false;
    }

    return stack->size == stack->limit;
}

double stack_growth(const stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return 0;
    }

    return stack->growth;
}

size_t stack_limit(const stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return 0;
    }

    return stack->limit;
}

bool stack_peek(const stack_t *stack, void *destination) {
    if (stack == NULL || destination == NULL || stack_empty(stack)) {
        errno = EINVAL;
        return false;
    }

    // errno set in _value_copy()
    return _value_copy(stack, stack->values[stack->size - 1], destination);
}

bool stack_pop(stack_t *stack, void *destination) {
    if (stack == NULL || stack_empty(stack)) {
        errno = EINVAL;
        return false;
    }

    if (destination != NULL && stack_peek(stack, destination) == false) {
        // errno set in stack_peek()
        return false;
    }

    _value_free(stack, stack->values[--stack->size]);

    return true;
}

bool stack_push(stack_t *stack, const void *value) {
    if (stack == NULL || value == NULL) {
        errno = EINVAL;
        return false;
    }

    if (stack_full(stack)) {
        errno = ENOSPC;
        return false;
    }

    if (stack_reserve(stack, stack->size + 1) == false) {
        // errno set in stack_reserve()
        return false;
    }

    void *new = _value_construct(stack, value);
    if (new == NULL) {
        // errno set in _value_construct()
        return false;
    }

    stack->values[stack->size++] = new;

    return true;
}

bool stack_reserve(stack_t *stack, size_t size) {
    if (stack == NULL || size > stack->limit) {
        errno = EINVAL;
        return false;
    }

    if (size <= stack->capacity) {
        return true;
    }

    size_t capacity = stack->capacity == 0 ? 1 : stack->capacity;
    while (capacity < size) {
        capacity *= stack->growth;

        if (capacity > stack->limit) {
            capacity = stack->limit;
        }

        // Detect overflow
        else if (capacity <= stack->capacity) {
            capacity = stack->limit;
        }
    }

    void **values = realloc(stack->values, capacity * sizeof(void *));
    if (values == NULL) {
        errno = ENOMEM;
        return false;
    }

    stack->values = values;
    stack->capacity = capacity;

    return true;
}

stack_t *stack_reverse(const stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return NULL;
    }

    stack_t *reversed = stack_construct(stack->width, stack->limit, stack->capacity, stack->growth, stack->copy, stack->free_);
    if (reversed == NULL) {
        // errno set in stack_construct()
        return NULL;
    }

    for (size_t i = stack->size; i > 0; i--) {
        if (stack_push(reversed, stack->values[i - 1]) == false) {
            stack_free(reversed);
            // errno set in stack_push()
            return NULL;
        }
    }

    return reversed;
}

size_t stack_size(const stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return 0;
    }

    return stack->size;
}

bool stack_trim(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return false;
    }

    if (stack_empty(stack)) {
        free(stack->values);
        stack->values = NULL;
    }
    else {
        void **values = realloc(stack->values, stack->size * sizeof(void *));
        if (values == NULL) {
            errno = ENOMEM;
            return false;
        }

        stack->values = values;
    }

    stack->capacity = stack->size;

    return true;
}

size_t stack_width(const stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return 0;
    }

    return stack->width;
}


static void *_value_construct(const stack_t *stack, const void *value) {
    if (stack == NULL || value == NULL) {
        errno = EINVAL;
        return NULL;
    }

    void *new = malloc(stack->width);
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (_value_copy(stack, value, new) == false) {
        free(new);
        // errno set in _value_copy()
        return NULL;
    }

    return new;
}

static bool _value_copy(const stack_t *stack, const void *value, void *destination) {
    if (stack == NULL || value == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (stack->copy != NULL) {
        // errno set in copy()
        return stack->copy(value, destination);
    }

    memcpy(destination, value, stack->width);
    return true;
}

static void _value_free(const stack_t *stack, void *value) {
    if (stack == NULL) {
        errno = EINVAL;
        return;
    }

    if (value != NULL) {
        if (stack->free_ != NULL) {
            stack->free_(value);
        }

        free(value);
    }
}
