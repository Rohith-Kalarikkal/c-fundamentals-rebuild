/**
 * @file    main.c
 * @brief   Entry point for the bit-manipulation study program.
 *
 * Runs four self-contained demonstrations in sequence:
 *   1. All six bitwise operators   (&, |, ^, ~, <<, >>)
 *   2. Set / Clear / Toggle / Check on a single byte
 *   3. Shift operators and BIT_MASK utility
 *   4. Full simulated peripheral register-control workflow
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include "bit_ops.h"
#include "register_sim.h"

/* ------------------------------------------------------------------ */
/*  Program entry                                                      */
/* ------------------------------------------------------------------ */

int main(void)
{
    printf("\n");
    printf("-------------------------------------------------------\n");
    printf("    BIT MANIPULATION - Embedded C Reference Program    \n");
    printf("    Covers: &  |  ^  ~  <<  >>  SET  CLR  TGL  CHK     \n");
    printf("-------------------------------------------------------\n");

    /* ---- Demo 1: bitwise operators -------------------------------- */
    bitwise_operators();

    /* ---- Demo 2: set / clear / toggle / check --------------------- */
    bit_operations();

    /* ---- Demo 3: shift operators ---------------------------------- */
    shift_operators();

    /* ---- Demo 4: peripheral register simulation ------------------- */
    register_simulation();

    printf("\n");
    printf("--------------------------------------------------------\n");
    printf("         All demos completed successfully.              \n");
    printf("--------------------------------------------------------\n\n");

    return 0;
}
