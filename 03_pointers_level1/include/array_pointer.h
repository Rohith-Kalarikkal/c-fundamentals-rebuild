/**
 * @file    array_pointer.h
 * @brief   Public API — array-vs-pointer comparison demo.
 *
 * @details Highlights:
 *          - How array names decay to pointers
 *          - Why sizeof behaves differently for arrays and pointers
 *          - That an array name is NOT re-assignable (it is not an lvalue)
 *          - Equivalent indexing: arr[i]  ↔  *(ptr + i)
 * 
 * @author    Rohith Kalarikkal Ramakrishnan
 * @version   1.0.0 
 */

#ifndef ARRAY_POINTER_H
#define ARRAY_POINTER_H

/**
 * @brief Prints a side-by-side comparison of array and pointer
 *        behaviour including sizeof, address equivalence, and
 *        element access syntax.
 */
void demo_array_vs_pointer(void);

#endif /* ARRAY_POINTER_H */
