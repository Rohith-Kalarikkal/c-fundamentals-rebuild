/**
 * @file    main.c
 * @brief   Entry point — C Pointers
 *
 * @details Runs each topic module in sequence.  Every module is
 *          self-contained; you can study, compile, and run them
 *          individually by commenting-out the calls you don't need.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0 
 */

#include <stdio.h>

#include "pointer_basics.h" 
#include "array_pointer.h" 
#include "array_ops.h"
#include "swap.h"

/* ─────────────────────────────────────────────────────────────────────────── */
int main(void)
{
    printf(" ------------------------------------------- \n");
    printf("|                 C POINTERS                |\n");
    printf(" ------------------------------------------- \n");

    /* Pointer declaration, address-of, dereference */
    demo_pointer_basics();

    /* Arithmetic: ++, --, +n, pointer difference */
    demo_pointer_arithmetic();

    /* Array vs pointer (sizeof, decay, re-assignment) */
    demo_array_vs_pointer();

    /* Reverse array + print via pointer increment */
    demo_array_ops();

    /* Swap two integers using pointers */
    demo_swap();

    printf("\n------------------------------------------\n");
    printf("  All topics complete.\n");
    printf("------------------------------------------\n\n");

    return 0;
}
