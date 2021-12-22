/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file vector.h
 * 
 * @brief Generic vector implementation
 */


#ifndef VECTOR_H
#define VECTOR_H


#include <stddef.h>
#include <stdbool.h>


typedef struct _Vector vector_t;
extern const size_t VECTOR_LIMIT;


/**
 * @brief Gets the vector capacity
 * @throw EINVAL If vector is NULL
 * 
 * @param[in] vector Vector reference
 * @return The vector capacity or 0 in errors
 */
size_t vector_capacity(vector_t *vector);

/**
 * @brief Removes all values from vector
 * @throw EINVAL If vector is NULL
 * 
 * @param[in, out] vector Vector reference
 * @return If the operation succeeded
 */
bool vector_clear(vector_t *vector);

/**
 * @brief Constructs a vector
 * @throw EINVAL If width or limit are 0, limit is greater than VECTOR_LIMIT, capacity is greater than limit or growth is lower than 2
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in] width Size of the value's data type
 * @param[in] limit Maximum number of values
 * @param[in] capacity Initial allocated space for values
 * @param[in] growth Capacity growth rate
 * @param[in] copy Function to copy a value (if NULL memcpy is used)
 * @param[in] free_ Function to free a value (if NULL no call is made)
 * @return Vector reference or NULL in errors
 */
vector_t *vector_construct(size_t width, size_t limit, size_t capacity, double growth, bool (*copy)(const void *source, void *destination), void (*free_)(void *value));

/**
 * @brief Tests if vector constains the key
 * @throw EINVAL If vector, key or compare are NULL
 * 
 * @param[in] vector Vector reference
 * @param[in] key Searched key reference
 * @param[in] compare Function witch returns 0 if value == key
 * @return If vector contains the key or false in errors
 */
bool vector_contains(vector_t *vector, const void *key, int (*compare)(const void *value, const void *key));

/**
 * @brief Copies the vector
 * @throw EINVAL If vector is NULL
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in] vector Vector reference
 * @return Copied vector reference or NULL in errors
 */
vector_t *vector_copy(vector_t *vector);

/**
 * @brief Tests if vector is empty
 * @throw EINVAL If vector is NULL
 * 
 * @param[in] vector Vector reference
 * @return If vector is empty or false in errors
 */
bool vector_empty(vector_t *vector);

/**
 * @brief Frees the memory used by vector
 * 
 * @param[in, out] vector Vector reference
 */
void vector_free(vector_t *vector);

/**
 * @brief Tests if vector is full
 * @throw EINVAL If vector is NULL
 * 
 * @param[in] vector Vector reference
 * @return If vector is full or false in errors
 */
bool vector_full(vector_t *vector);

/**
 * @brief Copies the value at index from vector to destination
 * @throw EINVAL If vector or destination are NULL or index is out of range
 * 
 * @param[in] vector Vector reference
 * @param[in] index Value index
 * @param[out] destination Destination address
 * @return If the operation succeeded
 */
bool vector_get(vector_t *vector, size_t index, void *destination);

/**
 * @brief Gets the vector growth
 * @throw EINVAL If vector is NULL
 * 
 * @param[in] vector Vector reference
 * @return The vector growth or 0 in errors
 */
double vector_growth(vector_t *vector);

/**
 * @brief Inserts the value at index in the vector
 * @throw EINVAL If vector or value are NULL or index is out of range + 1
 * @throw ENOSPC If vector is full
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in, out] vector Vector reference
 * @param[in] index Value index
 * @param[in] value Value reference
 * @return If the operation succeeded
 */
bool vector_insert(vector_t *vector, size_t index, const void *value);

/**
 * @brief Gets the vector limit
 * @throw EINVAL If vector is NULL
 * 
 * @param[in] vector Vector reference
 * @return The vector limit or 0 in errors
 */
size_t vector_limit(vector_t *vector);

/**
 * @brief Removes the value at index from the vector
 * @throw EINVAL If vector is NULL or index is out of range
 * 
 * @param[in, out] vector Vector reference
 * @param[in] index Value index
 * @param[out] destination Destination address (if NULL no copy is made)
 * @return If the operation succeeded
 */
bool vector_remove(vector_t *vector, size_t index, void *destination);

/**
 * @brief Removes all values equal to remove from the vector
 * @throw EINVAL If vector, remove or compare are NULL
 * 
 * @param[in, out] vector Vector reference
 * @param[in] remove Removed value reference
 * @param[in] compare Function witch returns 0 if value == remove
 * @return If the operation succeeded
 */
bool vector_removeall(vector_t *vector, const void *remove, int (*compare)(const void *value, const void *remove));

/**
 * @brief Requests the vector capacity to be at least size
 * @throw EINVAL If vector is NULL or size is greater than vector limit
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in, out] vector Vector reference
 * @param[in] size Minimum new capacity
 * @return If the operation succeeded
 */
bool vector_reserve(vector_t *vector, size_t size);

/**
 * @brief Copies the vector reversing the values' order
 * @throw EINVAL If vector is NULL
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in] vector Vector reference
 * @return Reversed vector reference or NULL in errors
 */
vector_t *vector_reverse(vector_t *vector);

/**
 * @brief Searches for key inside vector
 * @throw EINVAL If vector, key or compare are NULL
 * 
 * @param[in] vector Vector reference
 * @param[in] key Searched key reference
 * @param[in] compare Function witch returns 0 if value == key
 * @return Smallest key index or SIZE_MAX if not found or in errors
 */
size_t vector_search(vector_t *vector, const void *key, int (*compare)(const void *value, const void *key));

/**
 * @brief Sets the value at index in vector
 * @throw EINVAL If vector or value are NULL or index is out of range
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in, out] vector Vector reference
 * @param[in] index Value index
 * @param[in] value Value reference 
 * @return If the operation succeeded
 */
bool vector_set(vector_t *vector, size_t index, const void *value);

/**
 * @brief Gets the vector size
 * @throw EINVAL If vector is NULL
 * 
 * @param[in] vector Vector reference
 * @return The vector size or 0 in errors
 */
size_t vector_size(vector_t *vector);

/**
 * @brief Sorts the values in vector
 * @throw EINVAL If vector or compare are NULL
 * 
 * @param[in, out] vector Vector reference
 * @param[in] reverse If the order should be reversed relative to compare
 * @param[in] compare Function witch returns less than 0 if first comes before second
 * @return If the operation succeeded
 */
bool vector_sort(vector_t *vector, bool reverse, int (*compare)(const void *first, const void *second));

/**
 * @brief Reduces the vector capacity to match its size
 * @throw EINVAL If vector is NULL
 * @throw ENOMEM If there is not enough memory
 * 
 * @param[in, out] vector Vector reference
 * @return If the operation succeeded
 */
bool vector_trim(vector_t *vector);

/**
 * @brief Gets the vector width
 * @throw EINVAL If vector is NULL
 * 
 * @param[in] vector Vector reference
 * @return The vector width or 0 in errors
 */
size_t vector_width(vector_t *vector);


#endif // VECTOR_H
