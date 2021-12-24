/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file list.h
 * 
 * @brief Generic doubly linked list implementation
 */


#ifndef LIST_H
#define LIST_H


#include <stddef.h>
#include <stdbool.h>


typedef struct _List list_t;


/**
 * @brief Removes all values from the list
 * 
 * @param[in, out] list List reference
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If list is NULL
 */
bool list_clear(list_t *list);

/**
 * @brief Constructs a list
 * 
 * @param[in] width Size of the value's data type
 * @param[in] limit Maximum number of values
 * @param[in] copy Function to copy a value (if NULL memcpy is used)
 * @param[in] free_ Function to free a value (if NULL no call is made)
 * 
 * @return List reference or NULL in errors
 * 
 * @throw EINVAL If width or limit are 0
 * @throw ENOMEM If there is not enough memory
 */
list_t *list_construct(size_t width, size_t limit, bool (*copy)(const void *source, void *destination), void (*free_)(void *value));

/**
 * @brief Tests if the list contains the key
 * 
 * @param[in] list List reference
 * @param[in] key Searched key reference
 * @param[in] compare Function witch returns 0 if value equals key
 * 
 * @return If the list contains the key or false in errors
 * 
 * @throw EINVAL If list, key or compare are NULL
 */
bool list_contains(const list_t *list, const void *key, int (*compare)(const void *value, const void *key));

/**
 * @brief Copies the list
 * 
 * @param[in] list List reference
 * 
 * @return Copied list reference or NULL in errors
 * 
 * @throw EINVAL If list is NULL
 * @throw ENOMEM If there is not enough memory
 */
list_t *list_copy(const list_t *list);

/**
 * @brief Tests if list is empty
 * 
 * @param[in] list List reference
 * 
 * @return If list is empty or false in errors
 * 
 * @throw EINVAL If list is NULL
 */
bool list_empty(const list_t *list);

/**
 * @brief Frees the memory used by the list
 * 
 * @param[in, out] list List reference
 */
void list_free(list_t *list);

/**
 * @brief Tests if list is full
 * 
 * @param[in] list List reference
 * 
 * @return If list is full or false in errors
 * 
 * @throw EINVAL If list is NULL
 */
bool list_full(const list_t *list);

/**
 * @brief Copies the value at index from list to destination
 * 
 * @param[in] list List reference
 * @param[in] index Value index
 * @param[out] destination Destination address
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If list or destination are NULL or index is out of range
 */
bool list_get(const list_t *list, size_t index, void *destination);

/**
 * @brief Inserts the value at index in the list
 * 
 * @param[in, out] list List reference
 * @param[in] index Value index
 * @param[in] value Value reference
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If list or value are NULL or index is out of range + 1
 * @throw ENOSPC If list is full
 * @throw ENOMEM If there is not enough memory
 */
bool list_insert(list_t *list, size_t index, const void *value);

/**
 * @brief Gets the list limit
 * 
 * @param[in] list List reference
 * 
 * @return The list limit or 0 in errors
 * 
 * @throw EINVAL If list is NULL
 */
size_t list_limit(const list_t *list);

/**
 * @brief Removes the value at index from the list
 * 
 * @param[in, out] list List reference
 * @param[in] index Value index
 * @param[out] destination Destination address (if NULL no copy is made)
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If list is NULL or index is out of range
 */
bool list_remove(list_t *list, size_t index, void *destination);

/**
 * @brief Removes all values equal to remove from the list
 * 
 * @param[in, out] list List reference
 * @param[in] remove Removed value reference
 * @param[in] compare Function witch returns 0 if value equals remove
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If list, remove or compare are NULL
 */
bool list_removeall(list_t *list, const void *remove, int (*compare)(const void *value, const void *remove));

/**
 * @brief Copies the list reversing the values' order
 * 
 * @param[in] list List reference
 * 
 * @return Reversed list reference or NULL in errors
 * 
 * @throw EINVAL If list is NULL
 * @throw ENOMEM If there is not enough memory
 */
list_t *list_reverse(const list_t *list);

/**
 * @brief Searches for key inside list
 * 
 * @param[in] list List reference
 * @param[in] key Searched key reference
 * @param[in] compare Function witch returns 0 if value equals key
 * 
 * @return Smallest key index in list or SIZE_MAX if not found or in errors
 * 
 * @throw EINVAL If list, key or compare are NULL
 */
size_t list_search(const list_t *list, const void *key, int (*compare)(const void *value, const void *key));

/**
 * @brief Sets the value at index in list
 * 
 * @param[in, out] list List reference
 * @param[in] index Value index
 * @param[in] value Value reference
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If list or value are NULL or index is out of range
 * @throw ENOMEM If there is not enough memory
 */
bool list_set(list_t *list, size_t index, const void *value);

/**
 * @brief Gets the list size
 * 
 * @param[in] list List reference
 * 
 * @return The list size or 0 in errors
 * 
 * @throw EINVAL If list is NULL
 */
size_t list_size(const list_t *list);

/**
 * @brief Sorts the values in the list
 * 
 * @param[in, out] list List reference
 * @param[in] compare Function witch returns less than 0 if first comes before second
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If list or compare are NULL
 */
bool list_sort(list_t *list, int (*compare)(const void *first, const void *second));

/**
 * @brief Gets the list width
 * 
 * @param[in] list List reference
 * 
 * @return The list width or 0 in errors
 * 
 * @throw EINVAL If list is NULL
 */
size_t list_width(const list_t *list);


#endif // LIST_H
