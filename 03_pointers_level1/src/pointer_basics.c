/**
 * @file    pointer_basics.c
 * @brief   Basics Understanding on C pointers
 *
 * @details Pointer Basics:
 *          int  x   = 42;       -- ordinary int variable
 *          int *ptr = &x;       -- ptr holds the address of x  (& = address-of)
 *          int  val = *ptr;     -- val == 42  (* on a pointer = dereference)
 *           
 *          Key operators:
 *          &var   -> address of var   (produces a pointer value)
 *          *ptr   -> value at address (dereferences; gives the pointed-to object)
 *           
 *          Pointer Arithmetic:
 *          Arithmetic on a pointer moves it by multiples of sizeof(*pointer).
 *           
 *          int arr[] = {10, 20, 30};
 *          int *p    = arr;          -- p points to arr[0]
 *          p++;                      -- p now points to arr[1]  (+sizeof(int))
 *          p + 2                     -- address two elements ahead, no side-effect
 *          p2 - p1                   -- ptrdiff_t: number of elements between them
 * 
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include "pointer_basics.h"

static void print_section(const char *title)
{
    printf("\n==========================================\n");
    printf("  %s\n", title);
    printf("==========================================\n");
}

/* ------------------------------------------------------------------ */
void demo_pointer_basics(void)
{
    print_section("Pointer Basics");

    int x = 42;

    /*
     * Declare a pointer-to-int.
     * The type 'int *' tells the compiler how many bytes to read/write
     * when the pointer is dereferenced.
     */
    int *ptr = &x;          /* & -> "give me the address of x"    */

    printf("  Variable x          : value = %d\n", x);
    printf("  Address of x (&x)   : %p\n",  (void *)&x);
    printf("  Pointer ptr         : holds address %p\n", (void *)ptr);
    printf("  Dereference (*ptr)  : value at that address = %d\n", *ptr);

    /* ---------------------------------------------------------------- */
    /* Writing through a pointer -- this modifies x indirectly          */
    /* ---------------------------------------------------------------- */
    *ptr = 100;             /* equivalent to: x = 100;                  */
    printf("\n  After *ptr = 100:\n");
    printf("    x    = %d  (changed via pointer)\n", x);
    printf("    *ptr = %d  (same object, same value)\n", *ptr);

    /* ---------------------------------------------------------------- */
    /* Pointer to pointer (double indirection)                          */
    /* Embedded use-case: config tables, device register maps, etc.     */
    /* ---------------------------------------------------------------- */
    int **pptr = &ptr;      /* pointer to (pointer to int)              */
    printf("\n  Double indirection:\n");
    printf("    pptr   : %p  (holds address of ptr)\n",  (void *)pptr);
    printf("    *pptr  : %p  (holds address of x)\n",   (void *)*pptr);
    printf("    **pptr : %d  (value of x)\n", **pptr);

    /* ---------------------------------------------------------------- */
    /* NULL pointer -- always initialise unused pointers to NULL        */
    /* ---------------------------------------------------------------- */
    int *null_ptr = NULL;
    printf("\n  NULL pointer value  : %p\n", (void *)null_ptr);
    printf("  (Dereferencing NULL is undefined behaviour -- never do it)\n");
}

/* ------------------------------------------------------------------ */
void demo_pointer_arithmetic(void)
{
    print_section(" Pointer Arithmetic");

    int arr[] = {10, 20, 30, 40, 50};
    const size_t len = sizeof(arr) / sizeof(arr[0]);

    /*
     * When an array name is used in an expression it "decays" to a
     * pointer to its first element.  The pointer does NOT own the array.
     */
    int *p = arr;           /* equivalent to: p = &arr[0]               */

    printf("  sizeof(int)         = %zu bytes\n", sizeof(int));
    printf("  Base address (arr)  = %p\n\n", (void *)arr);

    /* ---------------------------------------------------------------- */
    /* Step through the array with ++                                   */
    /* ---------------------------------------------------------------- */
    printf("  Walking forward with p++:\n");
    size_t i;
    for (i = 0; i < len; i++) {
        printf("    p = %p  |  *p = %d  (arr[%zu])\n",
               (void *)p, *p, i);
        p++;                /* advances address by sizeof(int) bytes     */
    }

    /* ---------------------------------------------------------------- */
    /* Step backward with --                                            */
    /* ---------------------------------------------------------------- */
    p--;                    /* one-past-end; step back to last element   */
    printf("\n  Walking backward with p--:\n");
    for (i = len; i > 0; i--) {
        printf("    p = %p  |  *p = %d  (arr[%zu])\n",
               (void *)p, *p, i - 1);
        p--;
    }

    /* ---------------------------------------------------------------- */
    /* Offset notation: ptr + n                                         */
    /* ---------------------------------------------------------------- */
    p = arr;                /* reset to base                             */
    printf("\n  Offset notation (p + n):\n");
    for (i = 0; i < len; i++) {
        printf("    *(p + %zu) = %d\n", i, *(p + i));
    }

    /* ---------------------------------------------------------------- */
    /* Pointer difference                                               */
    /* Result type is ptrdiff_t (signed integer)                        */
    /* ---------------------------------------------------------------- */
    int *first = &arr[0];
    int *last  = &arr[len - 1];
    printf("\n  Pointer difference:\n");
    printf("    last - first = %td  (number of int elements between them)\n",
           last - first);
}
