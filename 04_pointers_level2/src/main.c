/**
 * @file    main.c
 * @brief   Entry point – orchestrates all pointer concept demonstrations.
 *
 *  This file is intentionally kept thin.
 *  All actual logic lives in the dedicated source modules so that
 *  each concept can be studied, compiled, and tested in isolation.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 *
 */

#include "double_ptr.h"
#include "struct_ptr.h"
#include "func_ptr.h"

#include <stdio.h>
#include <stdlib.h>


int main(void)
{
    printf(" ---------------------------------------------------------\n");
    printf("      Embedded C  -  Pointer Concepts Reference           \n");
    printf(" ---------------------------------------------------------\n");

    /* --------- Module 1: Double Pointers ------------------------- */
    double_ptr_basic();
    double_ptr_2d_matrix(3, 4);

    /* --------- Module 2: Struct Pointers + Dynamic Memory -------- */
    struct_ptr_stack();

    SensorNode_t *heap_sensor = struct_ptr_heap_alloc();
    if (heap_sensor != NULL) {
        /* Caller is responsible for freeing the returned pointer */
        printf("\n  [main] Updating heap sensor then freeing it.\n");
        sensor_node_update(heap_sensor, 0.0f, 0);
        sensor_node_print(heap_sensor);

        free(heap_sensor);
        heap_sensor = NULL;   /* Defensive NULL – prevents dangling pointer use */
        printf("  [main] heap_sensor freed and set to NULL.\n");
    }

    /* ── Module 3: Function Pointers ────────────────────────────────────── */
    func_ptr_basic();
    calculator(15.0, '+', 7.0);
    calculator(15.0, '/', 4.0);
    calculator(9.0,  '*', 3.0);
    calculator(5.0,  '/', 0.0);   /* Edge case: divide by zero */
    func_ptr_in_struct();
    event_callback();

    /* ── Done ───────────────────────────────────────────────────────────── */
    printf("\n ------------------------------------------ \n");
    printf(  "          Completed successfully.           \n");
    printf(  " ------------------------------------------ \n\n");

    return EXIT_SUCCESS;
}
