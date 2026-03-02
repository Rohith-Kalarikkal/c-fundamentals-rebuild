/**
 * @file    pointer_basics.h
 * @brief   Public API — pointer basics and pointer arithmetic demos.
 *
 * @details Covers:
 *          - Declaring a pointer and assigning an address
 *          - Dereferencing (reading / writing through a pointer)
 *          - Pointer arithmetic: increment, decrement, offset, difference
 *
 * @note    All demo functions print their output to stdout and have no
 *          side-effects outside the function scope.
 * 
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef POINTER_BASICS_H
#define POINTER_BASICS_H

/**
 * @brief Demonstrates pointer declaration, address-of (&), and
 *        dereference (*) operators.
 */
void demo_pointer_basics(void);

/**
 * @brief Demonstrates pointer arithmetic on an integer array:
 *        increment (ptr++), decrement (ptr--), offset (ptr + n),
 *        and pointer difference (ptr2 - ptr1).
 */
void demo_pointer_arithmetic(void);

#endif /* POINTER_BASICS_H */
