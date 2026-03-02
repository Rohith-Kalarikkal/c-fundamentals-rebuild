/**
 * @file    array_ops.h
 * @brief   Public API — array operations implemented with raw pointers.
 *
 * @details Covers:
 *          - In-place reversal using two-pointer (head / tail) technique
 *          - Printing an array by advancing a pointer rather than
 *            using an index variable
 * 
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0 
 */

#ifndef ARRAY_OPS_H
#define ARRAY_OPS_H

#include <stddef.h>   /* size_t */

/**
 * @brief  Reverses @p arr in-place using pointer manipulation.
 *
 * @param  arr   Pointer to the first element of the array.
 * @param  len   Number of elements in the array.
 *
 * @pre    arr != NULL && len > 0
 */
void reverse_array(int *arr, size_t len);

/**
 * @brief  Prints every element of @p arr by incrementing a pointer.
 *         Output format: "[ e0  e1  e2 ... eN-1 ]"
 *
 * @param  arr   Pointer to the first element of the array.
 * @param  len   Number of elements in the array.
 *
 * @pre    arr != NULL && len > 0
 */
void print_array_via_pointer(const int *arr, size_t len);

/**
 * @brief Convenience wrapper that calls reverse_array then
 *        print_array_via_pointer, preceded by a labelled header.
 */
void demo_array_ops(void);

#endif /* ARRAY_OPS_H */
