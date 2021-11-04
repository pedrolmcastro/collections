#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "stack.h"


struct _Stack {
    size_t size;
    size_t width;
    size_t limit;
    size_t capacity;
    double increment;
    void **data;
    void (*delete)(void *data);
    bool (*clone)(const void *source, void *destination);
};


const size_t STACK_LIMIT = SIZE_MAX / (2 * sizeof(void *)) - 1;


static bool _data_clone(stack_t *stack, const void *data, void *destination);
static void *_data_construct(stack_t *stack, const void *data);
static void _data_free(stack_t *stack, void *data);


stack_t *stack_construct(size_t width, size_t limit, size_t capacity, double increment, bool (*clone)(const void *source, void *destination), void (*delete)(void *data)) {
    if (width == 0 || limit == 0 || limit > STACK_LIMIT || capacity > limit || increment < 1.5) {
        errno = EINVAL;
        return NULL;
    }

    stack_t *stack = malloc(sizeof(stack_t));
    if (stack == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (capacity > 0) {
        stack->data = malloc(capacity * sizeof(void *));
        if (stack->data == NULL) {
            stack_free(stack);
            errno = ENOMEM;
            return NULL;
        }
    }
    else {
        stack->data = NULL;
    }

    stack->size = 0;

    stack->width = width;
    stack->limit = limit;
    stack->capacity = capacity;
    stack->increment = increment;
    stack->clone = clone;
    stack->delete = delete;

    return stack;
}

stack_t *stack_clone(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return NULL;
    }

    stack_t *clone = stack_construct(stack->width, stack->limit, stack->capacity, stack->increment, stack->clone, stack->delete);
    if (clone == NULL) {
        // errno set in stack_construct()
        return NULL;
    }

    for (size_t i = 0; i < stack->size; i++) {
        if (stack_push(clone, stack->data[i]) == false) {
            stack_free(clone);
            // errno set in stack_push()
            return NULL;
        }
    }
    
    return clone;
}

stack_t *stack_reverse(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return NULL;
    }

    stack_t *reversed = stack_construct(stack->width, stack->limit, stack->capacity, stack->increment, stack->clone, stack->delete);
    if (reversed == NULL) {
        // errno set in stack_construct()
        return NULL;
    }

    for (size_t i = stack->size; i > 0; i--) {
        if (stack_push(reversed, stack->data[i - 1]) == false) {
            stack_free(reversed);
            // errno set in stack_push()
            return NULL;
        }
    }

    return reversed;
}

void stack_free(stack_t *stack) {
    if (stack != NULL) {
        stack_clear(stack);
        stack_trim(stack);
        free(stack);
    }
}

bool stack_clear(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < stack->size; i++) {
        _data_free(stack, stack->data[i]);
    }

    stack->size = 0;

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
        capacity *= stack->increment;

        if (capacity > stack->limit) {
            capacity = stack->limit;
        }

        // Detect overflow
        else if (capacity <= stack->capacity) {
            capacity = stack->limit;
        }
    }

    void **data = realloc(stack->data, capacity * sizeof(void *));
    if (data == NULL) {
        errno = ENOMEM;
        return false;
    }

    stack->data = data;
    stack->capacity = capacity;

    return true;
}

bool stack_trim(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return false;
    }

    if (stack_empty(stack)) {
        free(stack->data);
        stack->data = NULL;
    }
    else {
        void **data = realloc(stack->data, stack->size * sizeof(void *));
        if (data == NULL) {
            errno = ENOMEM;
            return false;
        }

        stack->data = data;
    }

    stack->capacity = stack->size;

    return true;
}


bool stack_push(stack_t *stack, const void *data) {
    if (stack == NULL || data == NULL) {
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

    void *new = _data_construct(stack, data);
    if (new == NULL) {
        // errno set in _data_construct()
        return false;
    }

    stack->data[stack->size++] = new;

    return true;
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

    _data_free(stack, stack->data[--stack->size]);

    return true;
}

bool stack_peek(stack_t *stack, void *destination) {
    if (stack == NULL || destination == NULL || stack_empty(stack)) {
        errno = EINVAL;
        return false;
    }

    // errno set in _data_clone()
    return _data_clone(stack, stack->data[stack->size - 1], destination);
}


bool stack_contains(stack_t *stack, const void *key, int (*compare)(const void *data, const void *key)) {
    if (stack == NULL || key == NULL || compare == NULL) {
        errno = EINVAL;
        return false;
    }

    for (size_t i = 0; i < stack->size; i++) {
        if (compare(stack->data[i], key) == 0) {
            return true;
        }
    }

    return false;
}


size_t stack_size(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return 0;
    }

    return stack->size;
}

size_t stack_width(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return 0;
    }

    return stack->width;
}

size_t stack_limit(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return 0;
    }

    return stack->limit;
}

size_t stack_capacity(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return 0;
    }

    return stack->capacity;
}

double stack_increment(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return 0;
    }

    return stack->increment;
}


bool stack_empty(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return false;
    }

    return stack->size == 0;
}

bool stack_full(stack_t *stack) {
    if (stack == NULL) {
        errno = EINVAL;
        return false;
    }

    return stack->size == stack->limit;
}


static bool _data_clone(stack_t *stack, const void *data, void *destination) {
    if (stack == NULL || data == NULL || destination == NULL) {
        errno = EINVAL;
        return false;
    }

    if (stack->clone != NULL) {
        // errno set in clone()
        return stack->clone(data, destination);
    }

    memcpy(destination, data, stack->width);
    return true;
}

static void *_data_construct(stack_t *stack, const void *data) {
    if (stack == NULL || data == NULL) {
        errno = EINVAL;
        return NULL;
    }

    void *new = malloc(stack->width);
    if (new == NULL) {
        errno = ENOMEM;
        return NULL;
    }

    if (_data_clone(stack, data, new) == false) {
        free(new);
        // errno set in _data_clone()
        return NULL;
    }

    return new;
}

static void _data_free(stack_t *stack, void *data) {
    if (stack == NULL) {
        errno = EINVAL;
        return;
    }

    if (data != NULL) {
        if (stack->delete != NULL) {
            stack->delete(data);
        }

        free(data);
    }
}
