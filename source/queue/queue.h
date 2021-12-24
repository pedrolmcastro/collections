/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file queue.h
 * 
 * @brief Generic queue implementation using a singly linked list
 */


#ifndef QUEUE_H
#define QUEUE_H


#include <stddef.h>
#include <stdbool.h>


typedef struct _Queue queue_t;


/**
 * @brief Removes all values from the queue
 * 
 * @param[in, out] queue Queue reference
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If queue is NULL
 */
bool queue_clear(queue_t *queue);

/**
 * @brief Constructs a queue
 * 
 * @param[in] width Size of the value's data type
 * @param[in] limit Maximum number of values
 * @param[in] copy Function to copy a value (if NULL memcpy is used)
 * @param[in] free_ Function to free a value (if NULL no call is made)
 * 
 * @return Queue reference or NULL in errors
 * 
 * @throw EINVAL If width or limit are 0
 * @throw ENOMEM If there is not enough memory
 */
queue_t *queue_construct(size_t width, size_t limit, bool (*copy)(const void *source, void *destination), void (*free_)(void *value));

/**
 * @brief Tests if the queue contains the key
 * 
 * @param[in] queue Queue reference
 * @param[in] key Searched key reference
 * @param[in] compare Function witch returns 0 if value equals key
 * 
 * @return If the queue contains the key or false in errors
 * 
 * @throw EINVAL If queue, key or compare are NULL
 */
bool queue_contains(const queue_t *queue, const void *key, int (*compare)(const void *value, const void *key));

/**
 * @brief Copies the queue
 * 
 * @param[in] queue Queue reference
 * 
 * @return Copied queue reference or NULL in errors
 * 
 * @throw EINVAL If queue is NULL
 * @throw ENOMEM If there is not enough memory
 */
queue_t *queue_copy(const queue_t *queue);

/**
 * @brief Removes the front value of the queue
 * 
 * @param[in, out] queue Queue reference
 * @param[out] destination Destination address (if NULL no copy is made)
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If queue is NULL or empty
 */
bool queue_dequeue(queue_t *queue, void *destination);

/**
 * @brief Tests if queue is empty
 * 
 * @param[in] queue Queue reference
 * 
 * @return If queue is empty or false in errors
 * 
 * @throw EINVAL If queue is NULL
 */
bool queue_empty(const queue_t *queue);

/**
 * @brief Inserts the value at the back of the queue
 * 
 * @param[in, out] queue Queue reference
 * @param[in] value Value reference
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If queue or value are NULL
 * @throw ENOSPC If queue is full
 * @throw ENOMEM If there is not enough memory
 */
bool queue_enqueue(queue_t *queue, const void *value);

/**
 * @brief Frees the memory used by the queue
 * 
 * @param[in, out] queue Queue reference
 */
void queue_free(queue_t *queue);

/**
 * @brief Tests if queue is full
 * 
 * @param[in] queue Queue reference
 * 
 * @return If queue is full or false in errors
 * 
 * @throw EINVAL If queue is NULL
 */
bool queue_full(const queue_t *queue);

/**
 * @brief Gets the queue limit
 * 
 * @param[in] queue Queue reference
 * 
 * @return The queue limit or 0 in errors
 * 
 * @throw EINVAL If queue is NULL
 */
size_t queue_limit(const queue_t *queue);

/**
 * @brief Copies the front value from queue to destination
 * 
 * @param[in] queue Queue reference
 * @param[out] destination Destination address
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If queue or destination are NULL or queue is empty
 */
bool queue_peek(const queue_t *queue, void *destination);

/**
 * @brief Copies the queue reversing the values' order
 * 
 * @param[in] queue Queue reference
 * 
 * @return Reversed queue reference or NULL in errors
 * 
 * @throw EINVAL If queue is NULL
 * @throw ENOMEM If there is not enough memory
 */
queue_t *queue_reverse(const queue_t *queue);

/**
 * @brief Gets the queue size
 * 
 * @param[in] queue Queue reference
 * 
 * @return The queue size or 0 in errors
 * 
 * @throw EINVAL If queue is NULL
 */
size_t queue_size(const queue_t *queue);

/**
 * @brief Gets the queue width
 * 
 * @param[in] queue Queue reference
 * 
 * @return The queue width or 0 in errors
 * 
 * @throw EINVAL If queue is NULL
 */
size_t queue_width(const queue_t *queue);


#endif // QUEUE_H
