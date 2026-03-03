/**
 * @file    double_ptr.c
 * @brief   Double pointer (pointer-to-pointer) demonstrations.
 *
 *   CONCEPT: Double Pointer                                                
 *                                                                          
 *   A double pointer (**ptr) stores the ADDRESS of another pointer.        
 *                                                                          
 *   Memory layout example:                                                 
 *                                                                          
 *    int x = 42;                                                           
 *    int  *p  = &x;    // p  holds address of x  (e.g. 0x1000)           
 *    int **pp = &p;    // pp holds address of p  (e.g. 0x2000)           
 *                                                                          
 *    **pp == 42   (dereference twice to reach x)                          
 *    *pp  == 0x1000  (dereference once to reach p)                        
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "double_ptr.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* --------------------------- Private Helpers -------------------------- */

/**
 * @brief  Allocates a buffer and makes the caller's pointer point to it.
 *
 * @param  out_ptr  Address of the caller's pointer variable.
 * @param  value    Value to store in the allocated integer.
 *
 * @note   Caller is responsible for freeing *out_ptr.
 */
static void allocate_and_assign(int **out_ptr, int value)
{
    /* Allocate memory for one integer */
    *out_ptr = (int *)malloc(sizeof(int));
    if (*out_ptr == NULL) {
        fprintf(stderr, "[ERROR] malloc failed in %s\n", __func__);
        return;
    }
    **out_ptr = value;   /* Write through the double pointer */
}

/* ─────────────────────────── Public Functions ───────────────────────────── */

void double_ptr_basic(void)
{
    printf("\n ------------------------------------------ \n");
    printf(  "            Double Pointer Basics           \n");
    printf(  " ------------------------------------------ \n");

    int  value = 100;
    int *ptr   = &value;   /* Single pointer: points to value       */
    int **dptr = &ptr;     /* Double pointer: points to ptr         */

    printf("  value        = %d\n",   value);
    printf("  *ptr         = %d  (via single pointer)\n", *ptr);
    printf("  **dptr       = %d  (via double pointer)\n", **dptr);
    printf("  &value       = %p\n",   (void *)&value);
    printf("  ptr          = %p  (ptr stores address of value)\n", (void *)ptr);
    printf("  *dptr        = %p  (dptr stores address of ptr)\n",  (void *)*dptr);

    /* ------------------------------------------------------------------
     * Modifying the original value through the double pointer.
     * This is what makes double pointers useful: you can change both
     * the pointer itself AND the data it points to.
     * ------------------------------------------------------------------ */
    **dptr = 200;
    printf("\n  After **dptr = 200:\n");
    printf("  value = %d  (original variable modified via **dptr)\n", value);

    /* ------------------------------------------------------------------
     * Redirect what ptr points to by assigning through the double pointer.
     * ------------------------------------------------------------------ */
    int another = 999;
    *dptr = &another;   /* ptr now points to 'another', not 'value' */
    printf("\n  After *dptr = &another (redirect ptr):\n");
    printf("  *ptr  = %d  (ptr now points to 'another')\n", *ptr);
    printf("  value = %d  (unchanged)\n", value);

    /* ------------------------------------------------------------------
     * Out-parameter pattern: function allocates memory, caller receives ptr.
     * ------------------------------------------------------------------ */
    int *heap_int = NULL;
    allocate_and_assign(&heap_int, 42);
    printf("\n  Out-param alloc: *heap_int = %d  (addr=%p)\n",
           *heap_int, (void *)heap_int);
    free(heap_int);
    heap_int = NULL;   /* Always NULL after free – prevents dangling use */
    printf("  Memory freed; heap_int set to NULL.\n");
}

/* -------------------------------------------------------------------------- */

void double_ptr_2d_matrix(uint32_t rows, uint32_t cols)
{
    printf("\n ------------------------------------------ \n");
    printf(  "           2-D Matrix via Double Ptr        \n");
    printf(  " ------------------------------------------ \n");
    printf("  Allocating %u x %u matrix...\n", rows, cols);

    /* -------------------------------------------------------------------
     * Step 1: Allocate an array of ROW pointers.
     *         int **matrix is a pointer-to-pointer-to-int.
     * ------------------------------------------------------------------- */
    int **matrix = (int **)malloc(rows * sizeof(int *));
    if (matrix == NULL) {
        fprintf(stderr, "[ERROR] Row pointer allocation failed.\n");
        return;
    }

    /* -------------------------------------------------------------------
     * Step 2: Allocate each row's column storage independently.
     *         This gives us a truly dynamic 2-D array.
     * ------------------------------------------------------------------- */
    for (uint32_t r = 0; r < rows; r++) {
        matrix[r] = (int *)malloc(cols * sizeof(int));
        if (matrix[r] == NULL) {
            fprintf(stderr, "[ERROR] Column allocation failed at row %u.\n", r);
            /* Free already-allocated rows before returning */
            for (uint32_t k = 0; k < r; k++) {
                free(matrix[k]);
            }
            free(matrix);
            return;
        }
    }

    /* Fill: matrix[r][c] = r * cols + c  (makes values easy to verify) */
    for (uint32_t r = 0; r < rows; r++) {
        for (uint32_t c = 0; c < cols; c++) {
            matrix[r][c] = (int)(r * cols + c);
        }
    }

    /* Print the matrix */
    printf("  Matrix contents:\n");
    for (uint32_t r = 0; r < rows; r++) {
        printf("    Row %u: ", r);
        for (uint32_t c = 0; c < cols; c++) {
            printf("%4d", matrix[r][c]);
        }
        printf("\n");
    }

    /* -------------------------------------------------------------------
     * Step 3: Free in REVERSE order – rows first, then the pointer array.
     *         Failing to do this causes memory leaks (critical in embedded).
     * ------------------------------------------------------------------- */
    for (uint32_t r = 0; r < rows; r++) {
        free(matrix[r]);
        matrix[r] = NULL;
    }
    free(matrix);
    matrix = NULL;

    printf("  All matrix memory freed cleanly.\n");
}
