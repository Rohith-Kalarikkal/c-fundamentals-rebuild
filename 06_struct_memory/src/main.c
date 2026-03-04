/**
 * @file    src/main.c
 * @brief   Entry point for the struct padding & memory layout study project.
 *
 * Build
 * -----
 *   make          -> compile with -Wpadded (compiler warns on every inserted
 *                   padding byte — excellent for embedded register maps)
 *   make run      -> build + execute
 *   make sections -> show ELF section sizes via GNU size / objdump
 *
 * Learning path
 * -------------
 *   1. Run the program and study the output.
 *   2. Read each source file in the order listed below - each file is
 *      self-contained with detailed inline comments.
 *   3. Modify struct_variants.h: add a member or change its type, rerun,
 *      and observe how sizeof / offsets change.
 *   4. Enable -O2 in the Makefile and rerun - sizeof results are identical
 *      (padding is an ABI property, not an optimisation artefact).
 *
 * File reading order
 * ------------------
 *   include/struct_variants.h   → all type definitions & padding rationale
 *   include/alignment.h         → helper macros (ASSERT_SIZE, PRINT_OFFSET …)
 *   include/memory_sections.h   → section placement declarations
 *   src/alignment.c             → per-struct layout analysis
 *   src/struct_variants.c       → sizeof experiments & common pitfalls
 *   src/memory_sections.c       → .text / .data / .bss / stack demo
 *   docs/notes.md               → theory summary & quick-reference tables
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdint.h>

#include "alignment.h"
#include "memory_sections.h"
#include "struct_variants.h"

/* Forward declaration — defined in struct_variants.c */
void struct_variants_run_all(void);

int main(void)
{
    printf("------------------------------------------------------------\n");
    printf("            Struct Padding & Memory Layout                  \n");
    printf("------------------------------------------------------------\n");

    /* Alignment & padding analysis ---- */
    alignment_run_all();

    /* sizeof experiments & pitfalls --- */
    struct_variants_run_all();

    /* ELF section placement ----------─ */
    memory_sections_run();

    printf("\n--------------------------------------------------------------\n");
    printf("                         Completed.                              \n");
    printf("  --------------------------------------------------------------\n\n");

    return 0;
}
