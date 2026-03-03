/**
 * @file    double_ptr.h
 * @brief   Double pointer (pointer-to-pointer) demonstration interface.
 *
 * Covers:
 *   - Modifying a pointer via a double pointer (pass-by-pointer-to-pointer)
 *   - 2D array simulation using double pointers + dynamic memory
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef DOUBLE_PTR_H
#define DOUBLE_PTR_H

#include <stdint.h>

/* ----------------------- Public API ------------------------------- */

/**
 * @brief  Demonstrates modifying a pointer's value via a double pointer.
 *
 *  In embedded systems, double pointers appear in:
 *    - linked-list head manipulation
 *    - out-parameters that return heap-allocated buffers
 *    - driver init functions that populate a handle pointer
 */
void double_ptr_basic(void);

/**
 * @brief  Allocates a 2-D matrix using double pointers, fills it, then frees it.
 *
 * @param  rows   Number of rows
 * @param  cols   Number of columns
 */
void double_ptr_2d_matrix(uint32_t rows, uint32_t cols);

#endif /* DOUBLE_PTR_H */
