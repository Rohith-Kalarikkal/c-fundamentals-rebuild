/**
 * @file    swap.c
 * @brief   integer swap via pointers
 * 
 * @details Swap two integers via pointers.
 *
 *          C is strictly pass-by-value.  A function receives a COPY of its
 *          arguments.  To let a function mutate a caller's variable you must
 *          pass a pointer to that variable.
 *
 *          Wrong (pass by value -- no effect on caller):
 *          void bad_swap(int a, int b) { int t = a; a = b; b = t; }
 *
 *          Correct (pass by pointer -- modifies caller's variables):
 *          void swap(int *a, int *b)   { int t = *a; *a = *b; *b = t; }
 *
 *          Embedded relevance: toggling hardware registers, swapping DMA buffer
 *          descriptors, priority-queue rebalancing -- all use the same idiom.
 * 
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <assert.h>
#include "swap.h"

static void print_section(const char *title)
{
    printf("\n==========================================\n");
    printf("  %s\n", title);
    printf("==========================================\n");
}

/* ------------------------------------------------------------------ */
void swap_integers(int *a, int *b)
{
    /*
     * swapping a variable with itself is always a no-op, but
     * the XOR trick fails when a == b (both become 0), so we check.
     */
    assert(a != NULL);
    assert(b != NULL);

    if (a == b) return;             /* same address -> already "swapped" */

    /*
     * Classic temp-variable swap.  The compiler's optimiser generates
     * the same code as the XOR trick while remaining free of undefined
     * behaviour.
     */
    int tmp = *a;
    *a      = *b;
    *b      = tmp;
}

/* ------------------------------------------------------------------ */
void demo_swap(void)
{
    print_section("Swap Using Pointers");

    int x = 25;
    int y = 75;

    printf("\n  Before swap: x = %d,  y = %d\n", x, y);
    printf("  Calling swap_integers(&x, &y)...\n");

    /* Pass the ADDRESSES of x and y so the function can mutate them  */
    swap_integers(&x, &y);

    printf("  After  swap: x = %d,  y = %d\n", x, y);

    /* ---------------------------------------------------------------- */
    /* Visualise what happens inside swap_integers                      */
    /* ---------------------------------------------------------------- */
    printf("\n  Memory layout after swap:\n");
    printf("    &x = %p  -> holds %d\n", (void *)&x, x);
    printf("    &y = %p  -> holds %d\n", (void *)&y, y);

    printf("\n  Inside swap_integers(int *a, int *b):\n");
    printf("    a points to x : *a was %d\n", y);   /* y is original x */
    printf("    b points to y : *b was %d\n", x);
    printf("    int tmp = *a  -> save original *a\n");
    printf("    *a = *b       -> overwrite first with second\n");
    printf("    *b = tmp      -> overwrite second with saved value\n");

    /* ---------------------------------------------------------------- */
    /* Demonstrate self-swap guard                                      */
    /* ---------------------------------------------------------------- */
    int z = 42;
    printf("\n  Self-swap guard: swap_integers(&z, &z) -- should be no-op.\n");
    swap_integers(&z, &z);
    printf("    z is still %d  (OK)\n", z);

    /* ---------------------------------------------------------------- */
    /* Show why pass-by-value cannot swap                               */
    /* ---------------------------------------------------------------- */
    printf("\n  Why pass-by-value fails:\n");
    printf("    void bad_swap(int a, int b) modifies COPIES of x and y.\n");
    printf("    The originals in the caller remain untouched.\n");
    printf("    Always pass pointers when mutation of caller data is needed.\n");
}
