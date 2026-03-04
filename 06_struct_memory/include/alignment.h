/**
 * @file    include/alignment.h
 * @brief   Alignment inspection helpers, compile-time assertions, and runtime offset-printing utilities.
 *
 * Portability
 * -----------
 * Requires C11 (for _Static_assert and <stddef.h> alignof).
 * offsetof() is defined in <stddef.h> (C89+).
 * alignof()  is defined in <stdalign.h> (C11) or via __alignof__ (GCC/Clang).
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef INCLUDE_ALIGNMENT_H
#define INCLUDE_ALIGNMENT_H

#include <stddef.h>      /* offsetof, size_t                               */
#include <stdalign.h>    /* alignof  (C11)                                 */
#include <stdint.h>
#include <stdio.h>

/* --- Compile-time alignment assertion ---------------------------------- */
/**
 * @brief  Assert that TYPE is aligned to ALIGN bytes at compile time.
 *         Produces a clear diagnostic on violation.
 * @example  ASSERT_ALIGN(double, 8);
 */
#define ASSERT_ALIGN(TYPE, ALIGN) \
    _Static_assert(alignof(TYPE) == (ALIGN), \
        "Alignment mismatch: " #TYPE " expected align = " #ALIGN)

/* --- Compile-time size assertion --------------------------------------- */
/**
 * @brief  Assert that sizeof(TYPE) equals EXPECTED_SIZE.
 *         Catches inadvertent ABI breaks when adding struct members.
 * @example  ASSERT_SIZE(my_register_t, 4);
 */
#define ASSERT_SIZE(TYPE, EXPECTED_SIZE) \
    _Static_assert(sizeof(TYPE) == (EXPECTED_SIZE), \
        "Size mismatch: sizeof(" #TYPE ") != " #EXPECTED_SIZE)

/* --- Runtime field inspector ------------------------------------------ */
/**
 * @brief  Print the byte offset of FIELD inside STRUCT_VAR at runtime.
 *         Uses the actual variable (not type) so the macro is usable with
 *         anonymous structs / VLAs.
 */
#define PRINT_OFFSET(STRUCT_TYPE, FIELD) \
    printf("  offsetof(%-20s, %-12s) = %2zu bytes\n", \
           #STRUCT_TYPE, #FIELD, offsetof(STRUCT_TYPE, FIELD))

/* --- sizeof + alignof one-liner --------------------------------------- */
#define PRINT_SIZE_ALIGN(TYPE) \
    printf("  sizeof(%-24s) = %2zu  alignof = %zu\n", \
           #TYPE ")", sizeof(TYPE), alignof(TYPE))

/* --- Padding calculator ---------------------------------------------- */
/**
 * @brief  Calculate bytes wasted in a struct compared to a tightly-packed
 *         sum of its raw member sizes.
 * @param  struct_size   sizeof(the_struct)
 * @param  members_size  Sum of sizeof of all members without padding
 * @return Number of padding bytes inserted by the compiler
 */
static inline size_t calc_padding(size_t struct_size, size_t members_size)
{
    return (struct_size >= members_size) ? (struct_size - members_size) : 0U;
}

/* --- Section placement macros --------------------------------------- */
/* Place a variable explicitly in a named ELF section (GCC/Clang).      */
/* Useful for linker-script-driven placement in embedded targets.       */
#define IN_SECTION(sec)   __attribute__((section(sec)))
#define IN_TEXT           IN_SECTION(".text")
#define IN_DATA           IN_SECTION(".data")
#define IN_BSS            IN_SECTION(".bss")
#define IN_RODATA         IN_SECTION(".rodata")

/* --- Public function declarations ---------------------------------- */
void alignment_run_all(void);

#endif /* INCLUDE_ALIGNMENT_H */
