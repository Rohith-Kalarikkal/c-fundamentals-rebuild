/**
 * @file    array_ops.c
 * @brief   array operations implemented with raw pointers
 * 
 * @details Reverse Array Using Pointers (two-pointer technique):
 *
 *              head ->  [ A | B | C | D | E ]  <- tail
 *
 *              Step 1: swap *head and *tail
 *              Step 2: head++, tail--
 *              Step 3: repeat while head < tail
 *
 *              Time:  O(n/2) swaps -> O(n)
 *              Space: O(1) in-place, no extra buffer required
 *
 *          Print Array via Pointer Increment:
 *              Maintain a pointer and advance it instead of using an index variable.
 *              Equivalent machine code; the style is common in embedded firmware.
 * 
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0 
 */

#include <stdio.h>
#include <assert.h>
#include "array_ops.h"

static void print_section(const char *title)
{
    printf("\n==========================================\n");
    printf("  %s\n", title);
    printf("==========================================\n");
}

/* ------------------------------------------------------------------ */
void print_array_via_pointer(const int *arr, size_t len)
{
    assert(arr != NULL);
    assert(len > 0);

    const int *p   = arr;           /* p starts at first element        */
    const int *end = arr + len;     /* one-past-the-end sentinel        */

    printf("[ ");
    while (p < end) {               /* loop until p reaches sentinel    */
        printf("%d ", *p);
        p++;                        /* advance by sizeof(int) bytes     */
    }
    printf("]\n");
}

/* ------------------------------------------------------------------ */
void reverse_array(int *arr, size_t len)
{
    assert(arr != NULL);
    assert(len > 0);

    int *head = arr;                /* pointer to first element         */
    int *tail = arr + len - 1;     /* pointer to last element          */

    /*
     * Two-pointer convergence:
     * head and tail walk toward each other, swapping, until they meet.
     */
    while (head < tail) {
        int tmp = *head;            /* classic temp-variable swap       */
        *head   = *tail;
        *tail   = tmp;

        head++;                     /* advance toward centre            */
        tail--;                     /* retreat toward centre            */
    }
}

/* ------------------------------------------------------------------ */
void demo_array_ops(void)
{
    /* -------------------------------------------------------------- */
    /*              Print using pointer increment                     */
    /* -------------------------------------------------------------- */
    print_section("Print Array via Pointer Increment");

    int data[] = {7, 14, 21, 28, 35, 42};
    const size_t len = sizeof(data) / sizeof(data[0]);

    printf("\n  Array 'data'  : ");
    print_array_via_pointer(data, len);

    printf("\n  Trace of pointer walk:\n");
    const int *p   = data;
    const int *end = data + len;
    size_t idx = 0;
    while (p < end) {
        printf("    p = %p  |  *p = %2d  (index %zu)\n",
               (void *)p, *p, idx++);
        p++;
    }

    /* -------------------------------------------------------------- */
    /*                   Reverse array in-place                       */
    /* -------------------------------------------------------------- */
    print_section("Reverse Array Using Pointers");

    int rev[] = {10, 20, 30, 40, 50, 60, 70};
    const size_t rlen = sizeof(rev) / sizeof(rev[0]);

    printf("\n  Before reversal : ");
    print_array_via_pointer(rev, rlen);

    printf("\n  Two-pointer reversal steps:\n");
    {
        /* Show steps without modifying rev first */
        int *head = rev;
        int *tail = rev + rlen - 1;
        int  step = 1;
        while (head < tail) {
            printf("    Step %d: swap arr[%td]=%d  <->  arr[%td]=%d\n",
                   step++,
                   head - rev, *head,
                   tail - rev, *tail);
            head++;
            tail--;
        }
    }

    reverse_array(rev, rlen);

    printf("\n  After  reversal : ");
    print_array_via_pointer(rev, rlen);

    /* Edge case: single element (no-op) */
    int single[] = {99};
    reverse_array(single, 1);
    printf("\n  Single-element reverse : ");
    print_array_via_pointer(single, 1);
}
