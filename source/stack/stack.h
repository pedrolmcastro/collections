/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file stack.h
 * 
 * @brief Generic stack implementation using vector
 */


#ifndef STACK_H
#define STACK_H


#include <stddef.h>
#include <stdbool.h>


typedef struct _Stack stack_t;
extern const size_t STACK_LIMIT;


/**
 * @brief Gets the stack capacity
 * @throw EINVAL If stack is NULL
 * 
 * @param[in] stack Stack reference
 * @return The stack capacity or 0 in errors
 */
size_t stack_capacity(const stack_t *stack);

/**
 * @brief Removes all values from stack
 * @throw EINVAL If stack is NULL
 * 
 * @param[in, out] stack Stack reference
 * @return If the operation succeeded
 */
bool stack_clear(stack_t *stack);

/**
 * @brief Constructs a stack
 * @throw EINVAL If width or limit are 0, limit is greater than STACK_LIMIT, capacity is greater than limit or growth is lower than 2
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in] width Size of the value's data type
 * @param[in] limit Maximum number of values
 * @param[in] capacity Initial allocated space for values
 * @param[in] growth Capacity growth rate
 * @param[in] copy Function to copy a value (if NULL memcpy is used)
 * @param[in] free_ Function to free a value (if NULL no call is made)
 * @return Stack reference or NULL in errors
 */
stack_t *stack_construct(size_t width, size_t limit, size_t capacity, double growth, bool (*copy)(const void *source, void *destination), void (*free_)(void *value));

/**
 * @brief Tests if stack constains the key
 * @throw EINVAL If stack, key or compare are NULL
 * 
 * @param[in] stack Stack reference
 * @param[in] key Searched key reference
 * @param[in] compare Function witch returns 0 if value == key
 * @return If stack contains the key or false in errors
 */
bool stack_contains(const stack_t *stack, const void *key, int (*compare)(const void *value, const void *key));

/**
 * @brief Copies the stack
 * @throw EINVAL If stack is NULL
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in] stack Stack reference
 * @return Copied stack reference or NULL in errors
 */
stack_t *stack_copy(const stack_t *stack);

/**
 * @brief Tests if stack is empty
 * @throw EINVAL If stack is NULL
 * 
 * @param[in] stack Stack reference
 * @return If stack is empty or false in errors
 */
bool stack_empty(const stack_t *stack);

/**
 * @brief Frees the memory used by stack
 * 
 * @param[in, out] stack Stack reference
 */
void stack_free(stack_t *stack);

/**
 * @brief Tests if stack is full
 * @throw EINVAL If stack is NULL
 * 
 * @param[in] stack Stack reference
 * @return If stack is full or false in errors
 */
bool stack_full(const stack_t *stack);

/**
 * @brief Gets the stack growth
 * @throw EINVAL If stack is NULL
 * 
 * @param[in] stack Stack reference
 * @return The stack growth or 0 in errors
 */
double stack_growth(const stack_t *stack);

/**
 * @brief Gets the stack limit
 * @throw EINVAL If stack is NULL
 * 
 * @param[in] stack Stack reference
 * @return The stack limit or 0 in errors
 */
size_t stack_limit(const stack_t *stack);

/**
 * @brief Copies the top value from stack to destination
 * @throw EINVAL If stack or destination are NULL or stack is empty
 * 
 * @param[in] stack Stack reference
 * @param[out] destination Destination address
 * @return If the operation succeeded
 */
bool stack_peek(stack_t *stack, void *destination);

/**
 * @brief Removes the top value of the stack
 * @throw EINVAL If stack is NULL or empty
 * 
 * @param[in, out] stack Stack reference
 * @param[out] destination Destination address (if NULL no copy is made)
 * @return If the operation succeeded
 */
bool stack_pop(stack_t *stack, void *destination);

/**
 * @brief Inserts the value at the top of the stack
 * @throw EINVAL If stack or value are NULL
 * @throw ENOSPC If stack is full
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in, out] stack Stack reference
 * @param[in] value Value reference
 * @return If the operation succeeded
 */
bool stack_push(stack_t *stack, const void *value);

/**
 * @brief Requests the stack capacity to be at least size
 * @throw EINVAL If stack is NULL or size is greater than stack limit
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in, out] stack Stack reference
 * @param[in] size Minimum new capacity
 * @return If the operation succeeded
 */
bool stack_reserve(stack_t *stack, size_t size);

/**
 * @brief Copies the stack reversing the values' order
 * @throw EINVAL If stack is NULL
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in] stack Stack reference
 * @return Reversed stack reference or NULL in errors
 */
stack_t *stack_reverse(const stack_t *stack);

/**
 * @brief Gets the stack size
 * @throw EINVAL If stack is NULL
 * 
 * @param[in] stack Stack reference
 * @return The stack size or 0 in errors
 */
size_t stack_size(const stack_t *stack);

/**
 * @brief Reduces the stack capacity to match its size
 * @throw EINVAL If stack is NULL
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in, out] stack Stack reference
 * @return If the operation succeeded
 */
bool stack_trim(stack_t *stack);

/**
 * @brief Gets the stack width
 * @throw EINVAL If stack is NULL
 * 
 * @param[in] stack Stack reference
 * @return The stack width or 0 in errors
 */
size_t stack_width(const stack_t *stack);


#endif // STACK_H
