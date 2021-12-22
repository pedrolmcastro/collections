/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file deque.h
 * 
 * @brief Generic deque implementation using doubly linked list
 */


#ifndef DEQUE_H
#define DEQUE_H


#include <stddef.h>
#include <stdbool.h>


typedef struct _Deque deque_t;


/**
 * @brief Copies the back value from deque to destination
 * @throw EINVAL If deque or destination are NULL or deque is empty
 * 
 * @param[in] deque Deque reference
 * @param[out] destination Destination address
 * @return If the operation succeeded
 */
bool deque_back(const deque_t *deque, void *destination);

/**
 * @brief Removes all values from deque
 * @throw EINVAL If deque is NULL
 * 
 * @param[in, out] deque Deque reference
 * @return If the operation succeeded
 */
bool deque_clear(deque_t *deque);

/**
 * @brief Constructs a deque
 * @throw EINVAL If width or limit are 0
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in] width Size of the value's data type
 * @param[in] limit Maximum number of values
 * @param[in] copy Function to copy a value (if NULL memcpy is used)
 * @param[in] free_ Function to free a value (if NULL no call is made)
 * @return Deque reference or NULL in errors
 */
deque_t *deque_construct(size_t width, size_t limit, bool (*copy)(const void *source, void *destination), void (*free_)(void *value));

/**
 * @brief Tests if deque constains the key
 * @throw EINVAL If deque, key or compare are NULL
 * 
 * @param[in] deque Deque reference
 * @param[in] key Searched key reference
 * @param[in] compare Function witch returns 0 if value == key
 * @return If deque contains the key or false in errors
 */
bool deque_contains(const deque_t *deque, const void *key, int (*compare)(const void *value, const void *key));

/**
 * @brief Copies the deque
 * @throw EINVAL If deque is NULL
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in] deque Deque reference
 * @return Copied deque reference or NULL in errors
 */
deque_t *deque_copy(const deque_t *deque);

/**
 * @brief Tests if deque is empty
 * @throw EINVAL If deque is NULL
 * 
 * @param[in] deque Deque reference
 * @return If deque is empty or false in errors
 */
bool deque_empty(const deque_t *deque);

/**
 * @brief Frees the memory used by deque
 * 
 * @param[in, out] deque Deque reference
 */
void deque_free(deque_t *deque);

/**
 * @brief Copies the front value from deque to destination
 * @throw EINVAL If deque or destination are NULL or deque is empty
 * 
 * @param[in] deque Deque reference
 * @param[out] destination Destination address
 * @return If the operation succeeded
 */
bool deque_front(const deque_t *deque, void *destination);

/**
 * @brief Tests if deque is full
 * @throw EINVAL If deque is NULL
 * 
 * @param[in] deque Deque reference
 * @return If deque is full or false in errors
 */
bool deque_full(const deque_t *deque);

/**
 * @brief Gets the deque limit
 * @throw EINVAL If deque is NULL
 * 
 * @param[in] deque Deque reference
 * @return The deque limit or 0 in errors
 */
size_t deque_limit(const deque_t *deque);

/**
 * @brief Removes the back value of the deque
 * @throw EINVAL If deque is NULL or empty
 * 
 * @param[in, out] deque Deque reference
 * @param[out] destination Destination address (if NULL no copy is made)
 * @return If the operation succeeded
 */
bool deque_pop(deque_t *deque, void *destination);

/**
 * @brief Inserts the value at the back of the deque
 * @throw EINVAL If deque or value are NULL
 * @throw ENOSPC If deque is full
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in, out] deque Deque reference
 * @param[in] value Value reference
 * @return If the operation succeeded
 */
bool deque_push(deque_t *deque, const void *value);

/**
 * @brief Removes the front value of the deque
 * @throw EINVAL If deque is NULL or empty
 * 
 * @param[in, out] deque Deque reference
 * @param[out] destination Destination address (if NULL no copy is made)
 * @return If the operation succeeded
 */
bool deque_shift(deque_t *deque, void *destination);

/**
 * @brief Gets the deque size
 * @throw EINVAL If deque is NULL
 * 
 * @param[in] deque Deque reference
 * @return The deque size or 0 in errors
 */
size_t deque_size(const deque_t *deque);

/**
 * @brief Inserts the value at the front of the deque
 * @throw EINVAL If deque or value are NULL
 * @throw ENOSPC If deque is full
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in, out] deque Deque reference
 * @param[in] value Value reference
 * @return If the operation succeeded
 */
bool deque_unshift(deque_t *deque, const void *value);

/**
 * @brief Gets the deque width
 * @throw EINVAL If deque is NULL
 * 
 * @param[in] deque Deque reference
 * @return The deque width or 0 in errors
 */
size_t deque_width(const deque_t *deque);


#endif // DEQUE_H
