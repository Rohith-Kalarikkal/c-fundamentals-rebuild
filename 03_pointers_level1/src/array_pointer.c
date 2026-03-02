/**
 * @file      array_pointer.c
 * @brief     array vs pointer demonstration
 * 
 * @details   Array vs Pointer -- Key Distinctions
 *            SIMILARITY
 *            An array name in an expression decays to a pointer to arr[0].
 *            So  arr[i]  and  *(ptr + i)  compile to identical machine code. 
 * 
 *            CRITICAL DIFFERENCES
 *            1. sizeof
 *                   sizeof(arr)   -> total bytes of the array  (e.g. 5 * 4 = 20)
 *                   sizeof(ptr)   -> size of the pointer only  (4 or 8 bytes)
 *
 *            2. Re-assignment
 *                   arr = something;  <- ILLEGAL  (array name is not an lvalue)
 *                   ptr = something;  <- Legal    (pointer variable can be re-pointed)
 *
 *            3. Storage
 *                   Array: contiguous block on the stack (or static memory)
 *                   Pointer: single word holding an address; may point anywhere
 *
 *            4. Lifetime / ownership
 *                   The array owns its storage; the pointer borrows someone else's.
 *
 * @author    Rohith Kalarikkal Ramakrishnan
 * @version   1.0.0 
 * 
 */

#include <stdio.h>
#include "array_pointer.h"

static void print_section(const char *title)
{
    printf("\n==========================================\n");
    printf("  %s\n", title);
    printf("==========================================\n");
}

/* ------------------------------------------------------------------ */
void demo_array_vs_pointer(void)
{
    print_section("Array vs Pointer");

    int arr[] = {1, 2, 3, 4, 5};           /* array: 5 ints on the stack  */
    int *ptr  = arr;                        /* pointer: points to arr[0]   */

    /* ---------------------------------------------------------------- */
    /* 1. sizeof                                                        */
    /* ---------------------------------------------------------------- */
    printf("\n  [1] sizeof comparison:\n");
    printf("    sizeof(arr)  = %zu bytes  <- entire array (%zu elements x %zu bytes)\n",
           sizeof(arr), sizeof(arr)/sizeof(arr[0]), sizeof(arr[0]));
    printf("    sizeof(ptr)  = %zu bytes  <- just the pointer variable\n",
           sizeof(ptr));

    /* ---------------------------------------------------------------- */
    /* 2. Address equality -- arr decays to &arr[0] in expressions     */
    /* ---------------------------------------------------------------- */
    printf("\n  [2] Address equivalence (array name decays to &arr[0]):\n");
    printf("    arr           = %p\n", (void *)arr);
    printf("    &arr[0]       = %p  (identical)\n", (void *)&arr[0]);
    printf("    ptr           = %p  (same address, different variable)\n",
           (void *)ptr);

    /*
     * Subtle: &arr has the same numeric value but a DIFFERENT type.
     * &arr is int (*)[5] -- pointer to the whole array.
     * Incrementing it jumps by sizeof(arr), not sizeof(int).
     */
    printf("    &arr          = %p  (same address; type: int (*)[5])\n",
           (void *)&arr);
    printf("    &arr + 1      = %p  (jumps by sizeof(arr) = %zu bytes!)\n",
           (void *)(&arr + 1), sizeof(arr));
    printf("    ptr  + 1      = %p  (jumps by sizeof(int) = %zu bytes)\n",
           (void *)(ptr + 1), sizeof(int));

    /* ---------------------------------------------------------------- */
    /* 3. Element access -- arr[i] is syntactic sugar for *(arr + i)   */
    /* ---------------------------------------------------------------- */
    printf("\n  [3] Equivalent element access syntax:\n");
    size_t n = sizeof(arr) / sizeof(arr[0]);
    size_t i;
    for (i = 0; i < n; i++) {
        printf("    arr[%zu]=%d  *(arr+%zu)=%d  ptr[%zu]=%d  *(ptr+%zu)=%d\n",
               i, arr[i],
               i, *(arr + i),
               i, ptr[i],
               i, *(ptr + i));
    }

    /* ---------------------------------------------------------------- */
    /* 4. Pointer can be re-assigned; array name cannot                 */
    /* ---------------------------------------------------------------- */
    printf("\n  [4] Re-assignment:\n");
    int  other[] = {99, 88, 77};
    ptr          = other;       /* OK: pointer re-pointed to new array   */
    printf("    ptr re-pointed to 'other': ptr[0]=%d, ptr[1]=%d, ptr[2]=%d\n",
           ptr[0], ptr[1], ptr[2]);
    printf("    arr = other;  <- would be a COMPILE ERROR (not an lvalue)\n");

    /* ---------------------------------------------------------------- */
    /* 5. Passing to a function -- arrays always decay to pointer       */
    /* ---------------------------------------------------------------- */
    printf("\n  [5] Array parameter decay rule:\n");
    printf("    void fn(int arr[])  is IDENTICAL to  void fn(int *arr)\n");
    printf("    Inside fn, sizeof(arr) == sizeof(ptr), NOT the full array.\n");
    printf("    Always pass the length separately (or use a struct wrapper).\n");
}
