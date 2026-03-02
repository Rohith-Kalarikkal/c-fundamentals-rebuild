/**
 * @file    swap.h
 * @brief   Public API — integer swap via pointers.
 *
 * @details Demonstrates why pass-by-pointer is required in C to
 *          mutate variables inside a called function (C has no
 *          pass-by-reference like C++).
 * 
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef SWAP_H
#define SWAP_H

/**
 * @brief  Swaps the two integers pointed to by @p a and @p b.
 *
 * @param  a   Non-null pointer to the first integer.
 * @param  b   Non-null pointer to the second integer.
 *
 * @pre    a != NULL && b != NULL && a != b
 */
void swap_integers(int *a, int *b);

/**
 * @brief  Prints a before/after comparison to demonstrate swap_integers.
 */
void demo_swap(void);

#endif /* SWAP_H */
