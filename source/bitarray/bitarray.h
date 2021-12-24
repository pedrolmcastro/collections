/**
 * @author Pedro Lucas de Moliner de Castro
 * @copyright MIT License
 * @file bitarray.h
 * 
 * @brief Bitarray implementation using bytes as buckets
 */


#ifndef BITARRAY_H
#define BITARRAY_H


#include <stddef.h>
#include <stdbool.h>


typedef struct _BitArray bitarray_t;


/**
 * @brief Tests if all bits in the bitarray are set to 1
 * 
 * @param[in] bitarray Bitarray reference
 * 
 * @return If all bits in the bitarray are set to 1 or false in errors
 * 
 * @throw EINVAL If bitarray is NULL
 */
bool bitarray_all(const bitarray_t *bitarray);

/**
 * @brief Performs an and operation
 * 
 * @param[in] first First operand reference
 * @param[in] second Second operand reference
 * 
 * @return Resultant bitarray reference or NULL in errors
 * 
 * @throw EINVAL If first or second are NULL
 * @throw ENOMEM If there is not enough memory
 */
bitarray_t *bitarray_and(const bitarray_t *first, const bitarray_t *second);

/**
 * @brief Tests if any bit in the bitarray is set to 1
 * 
 * @param[in] bitarray Bitarray reference
 * 
 * @return If any bit in the bitarray is set to 1 or false in errors
 * 
 * @throw EINVAL If bitarray is NULL
 */
bool bitarray_any(const bitarray_t *bitarray);

/**
 * @brief Gets the bitarray buckets
 * 
 * @param[in] bitarray Bitarray reference
 * 
 * @return The bitarray buckets or 0 in errors
 * 
 * @throw EINVAL If bitarray is NULL
 */
size_t bitarray_buckets(const bitarray_t *bitarray);

/**
 * @brief Resets to 0 all bits in the bitarray
 * 
 * @param[in, out] bitarray Bitarray reference
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If bitarray is NULL
 */
bool bitarray_clear(bitarray_t *bitarray);

/**
 * @brief Constructs a bitarray initialized with 0
 * 
 * @param[in] size Number of bits
 * 
 * @return Bitarray reference or NULL in errors
 * 
 * @throw EINVAL If size is 0
 * @throw ENOMEM If there is not enough memory
 */
bitarray_t *bitarray_construct(size_t size);

/**
 * @brief Copies the bitarray
 * 
 * @param[in] bitarray Bitarray reference
 * 
 * @return Copied bitarray reference or NULL in errors
 * 
 * @throw EINVAL If bitarray is NULL
 * @throw ENOMEM If there is not enough memory
 */
bitarray_t *bitarray_copy(const bitarray_t *bitarray);

/**
 * @brief Counts the number of bits set to 1 in the bitarray
 * 
 * @param[in] bitarray Bitarray reference
 * 
 * @return Number of bits set to 1 in bitarray or 0 in errors
 * 
 * @throw EINVAL If bitarray is NULL
 */
size_t bitarray_count(const bitarray_t *bitarray);

/**
 * @brief Sets to 1 all bits in the bitarray
 * 
 * @param[in, out] bitarray Bitarray reference
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If bitarray is NULL
 */
bool bitarray_fill(bitarray_t *bitarray);

/**
 * @brief Flips the bit at index
 * 
 * @param[in, out] bitarray Bitarray reference
 * @param[in] index Bit index
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If bitarray is NULL or index is out of range
 */
bool bitarray_flip(bitarray_t *bitarray, size_t index);

/**
 * @brief Frees the memory used by the bitarray
 * 
 * @param[in, out] bitarray Bitarray reference
 */
void bitarray_free(bitarray_t *bitarray);

/**
 * @brief Tests if no bit in the bitarray is set to 1
 * 
 * @param[in] bitarray Bitarray reference
 * 
 * @return If no bit in the bitarray is set to 1 or false in errors
 * 
 * @throw EINVAL If bitarray is NULL
 */
bool bitarray_none(const bitarray_t *bitarray);

/**
 * @brief Performs a not operation
 * 
 * @param[in] bitarray Operand reference
 * 
 * @return Resultant bitarray reference or NULL in errors
 * 
 * @throw EINVAL If bitarray is NULL
 * @throw ENOMEM If there is not enough memory
 */
bitarray_t *bitarray_not(const bitarray_t *bitarray);

/**
 * @brief Performs an or operation
 * 
 * @param[in] first First operand reference
 * @param[in] second Second operand reference
 * 
 * @return Resultant bitarray reference or NULL in errors
 * 
 * @throw EINVAL If first or second are NULL
 * @throw ENOMEM If there is not enough memory
 */
bitarray_t *bitarray_or(const bitarray_t *first, const bitarray_t *second);

/**
 * @brief Resets to 0 the bit at index
 * 
 * @param[in, out] bitarray Bitarray reference
 * @param[in] index Bit index
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If bitarray is NULL or index is out of range
 */
bool bitarray_reset(bitarray_t *bitarray, size_t index);

/**
 * @brief Sets to 1 the bit at index
 * 
 * @param[in, out] bitarray Bitarray reference
 * @param[in] index Bit index
 * 
 * @return If the operation succeeded
 * 
 * @throw EINVAL If bitarray is NULL or index is out of range
 */
bool bitarray_set(bitarray_t *bitarray, size_t index);

/**
 * @brief Gets the bitarray size
 * 
 * @param[in] bitarray Bitarray reference
 * 
 * @return The bitarray size or 0 in errors
 * 
 * @throw EINVAL If bitarray is NULL
 */
size_t bitarray_size(const bitarray_t *bitarray);

/**
 * @brief Tests if the bit at index is set to 1
 * 
 * @param[in] bitarray Bitarray reference
 * @param[in] index Bit index
 * 
 * @return If the bit at index is set to 1 or false in errors
 * 
 * @throw EINVAL If bitarray is NULL or index is out of range
 */
bool bitarray_test(const bitarray_t *bitarray, size_t index);

/**
 * @brief Performs a xor operation
 * 
 * @param[in] first First operand reference
 * @param[in] second Second operand reference
 * 
 * @return Resultant bitarray reference or NULL in errors
 * 
 * @throw EINVAL If first or second are NULL
 * @throw ENOMEM If there is not enough memory
 */
bitarray_t *bitarray_xor(const bitarray_t *first, const bitarray_t *second);


#endif // BITARRAY_H
