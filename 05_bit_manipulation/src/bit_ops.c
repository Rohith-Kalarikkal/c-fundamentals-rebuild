/**
 * @file    bit_ops.c
 * @brief   Implementation of bit-manipulation utility functions.
 *
 * Contains the bodies for print_binary(), demo_bitwise_operators(),
 * demo_bit_operations(), and demo_shift_operators().  All heavy lifting
 * is done by the macros defined in bit_ops.h.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>   /* memset */

#include "bit_ops.h"


/**
 * @brief  Convert a uint32_t to a zero-padded binary string with
 *         underscore separators every 4 bits for readability.
 *         Result stored in caller-supplied buffer (must be >= 40 bytes).
 *
 * @param  value   Input value.
 * @param  buf     Output character buffer (caller allocates).
 * @param  bits    Number of bits to display (8, 16, or 32).
 */
static void uint_to_binstr(uint32_t value, char *buf, uint8_t bits)
{
    uint8_t  out_idx = 0;
    int8_t   bit_idx;

    for (bit_idx = (int8_t)(bits - 1); bit_idx >= 0; --bit_idx) {
        buf[out_idx++] = (char)('0' + ((value >> bit_idx) & 1UL));

        /* Insert underscore after every 4th bit (except at the end). */
        if ((bit_idx > 0) && (bit_idx % 4 == 0)) {
            buf[out_idx++] = '_';
        }
    }
    buf[out_idx] = '\0';
}

/* ------------------------------------------------------------------ */
/*  Public utility                                                     */
/* ------------------------------------------------------------------ */

void print_binary(const char *label, uint32_t value)
{
    char binstr[40]; /* 32 bits + 7 underscores + null = 40 chars */
    uint_to_binstr(value, binstr, 32);
    printf("  %-30s : 0b %s  (0x%08X  / %10u)\n",
           label, binstr, value, value);
}

/* ------------------------------------------------------------------ */
/*  All six bitwise operators                                         */
/* ------------------------------------------------------------------ */

void bitwise_operators(void)
{
    /*
     * Choose two operands whose bit patterns make every operator's
     * effect visually obvious in the binary printout.
     *
     *   A = 0b 1010_1010  (0xAA) — alternating bits
     *   B = 0b 1111_0000  (0xF0) — high nibble set
     */
    uint8_t A = 0xAAU;   /* 1010 1010 */
    uint8_t B = 0xF0U;   /* 1111 0000 */

    printf("\n");
    printf("-------------------------------------------------------\n");
    printf("              Six Bitwise Operators                    \n");
    printf("-------------------------------------------------------\n");

    printf("\n  Operands:\n");
    print_binary("A (0xAA)", A);
    print_binary("B (0xF0)", B);

    /* ---- AND (&) -------------------------------------------------- */
    printf("\n  1. AND  ( A & B )\n");
    printf("     Rule: output bit = 1 only when BOTH inputs are 1.\n");
    printf("     Use case: masking - isolate specific bits.\n");
    print_binary("A & B", (uint32_t)(A & B));

    /* ---- OR  (|) -------------------------------------------------- */
    printf("\n  2. OR   ( A | B )\n");
    printf("     Rule: output bit = 1 when AT LEAST ONE input is 1.\n");
    printf("     Use case: setting bits without disturbing others.\n");
    print_binary("A | B", (uint32_t)(A | B));

    /* ---- XOR (^) -------------------------------------------------- */
    printf("\n  3. XOR  ( A ^ B )\n");
    printf("     Rule: output bit = 1 when inputs DIFFER.\n");
    printf("     Use case: toggling bits, detecting changes.\n");
    print_binary("A ^ B", (uint32_t)(A ^ B));

    /* ---- NOT (~) -------------------------------------------------- */
    printf("\n  4. NOT  ( ~A )  - bitwise complement\n");
    printf("     Rule: every bit is flipped.\n");
    printf("     Note: result is 32-bit, mask to 8-bit for clarity.\n");
    print_binary("~A (full 32-bit)", (uint32_t)(~A));
    print_binary("~A (masked 8-bit)", (uint32_t)((uint8_t)(~A)));

    /* ---- LEFT SHIFT (<<) ------------------------------------------ */
    printf("\n  5. LEFT SHIFT  ( A << 2 )\n");
    printf("     Rule: shift bits toward MSB; fill LSBs with 0.\n");
    printf("     Effect: multiply by 2^n (if no bits are lost).\n");
    print_binary("A << 2", (uint32_t)((uint8_t)(A << 2)));

    /* ---- RIGHT SHIFT (>>) ----------------------------------------- */
    printf("\n  6. RIGHT SHIFT ( B >> 4 )\n");
    printf("     Rule (unsigned): shift toward LSB, fill MSBs with 0.\n");
    printf("     Effect: divide by 2^n (integer, floor).\n");
    print_binary("B >> 4", (uint32_t)(B >> 4));
}

/* ------------------------------------------------------------------ */
/*  Set / Clear / Toggle / Check on a single byte                     */
/* ------------------------------------------------------------------ */

void bit_operations(void)
{
    uint8_t reg = 0x00U;  /* Start with all bits cleared */
    bool    is_set;

    printf("\n");
    printf("------------------------------------------------------\n");
    printf("          Set / Clear / Toggle / Check                \n");
    printf("------------------------------------------------------\n");

    printf("\n  Initial state: reg = 0x00\n");
    print_binary("reg", reg);

    /* ----- SET_BIT ------------------------------------------------- */
    printf("\n  >> SET_BIT(reg, 3)  - raise bit 3\n");
    SET_BIT(reg, 3);
    print_binary("reg after set", reg);

    printf("\n  >> SET_BIT(reg, 7)  - raise bit 7\n");
    SET_BIT(reg, 7);
    print_binary("reg after set", reg);

    printf("\n  >> SET_BIT(reg, 0)  - raise bit 0 (LSB)\n");
    SET_BIT(reg, 0);
    print_binary("reg after set", reg);

    /* ----- CHECK_BIT ----------------------------------------------- */
    printf("\n  >> CHECK_BIT - inspect individual bits\n");
    is_set = (bool)CHECK_BIT(reg, 3);
    printf("     CHECK_BIT(reg, 3) = %d  (%s)\n", is_set, is_set ? "SET" : "CLEAR");
    is_set = (bool)CHECK_BIT(reg, 5);
    printf("     CHECK_BIT(reg, 5) = %d  (%s)\n", is_set, is_set ? "SET" : "CLEAR");

    /* ----- CLEAR_BIT ----------------------------------------------- */
    printf("\n  >> CLEAR_BIT(reg, 3) - lower bit 3\n");
    CLEAR_BIT(reg, 3);
    print_binary("reg after clear", reg);

    printf("\n  >> CLEAR_BIT(reg, 7) - lower bit 7\n");
    CLEAR_BIT(reg, 7);
    print_binary("reg after clear", reg);

    /* ----- TOGGLE_BIT ---------------------------------------------- */
    printf("\n  >> TOGGLE_BIT(reg, 0) - flip bit 0 (currently SET -> should CLEAR)\n");
    TOGGLE_BIT(reg, 0);
    print_binary("reg after toggle", reg);

    printf("\n  >> TOGGLE_BIT(reg, 4) - flip bit 4 (currently CLEAR -> should SET)\n");
    TOGGLE_BIT(reg, 4);
    print_binary("reg after toggle", reg);

    printf("\n  >> TOGGLE_BIT(reg, 4) again - flip back to CLEAR\n");
    TOGGLE_BIT(reg, 4);
    print_binary("reg after toggle", reg);

    /* ----- Multi-bit field ----------------------------------------- */
    printf("\n  >> SET_FIELD(reg, pos=4, width=3, val=0b101)\n");
    printf("     Writing value 5 (0b101) into bits [6:4]\n");
    SET_FIELD(reg, 4, 3, 5U);
    print_binary("reg after SET_FIELD", reg);

    uint32_t extracted = GET_FIELD(reg, 4, 3);
    printf("     GET_FIELD(reg, 4, 3) = %u  (expected 5)\n", extracted);
}

/* ------------------------------------------------------------------ */
/*         Shift operators — multiplication / division trick          */
/* ------------------------------------------------------------------ */

void shift_operators(void)
{
    uint8_t  shift;

    printf("\n");
    printf("------------------------------------------------------\n");
    printf("     Left Shift as Fast Multiply by Power-of-2        \n");
    printf("------------------------------------------------------\n");

    printf("\n  val = 1; left-shift up to bit 7:\n");
    printf("  %-6s  %-12s  %-10s\n", "Shift", "Expression", "Value");
    printf("  %-6s  %-12s  %-10s\n", "------", "----------", "-----");

    for (shift = 0U; shift <= 7U; ++shift) {
        printf("  << %-3u  1 << %-6u  %-10u\n", shift, shift, 1U << shift);
    }

    printf("\n  Right-shift - fast integer divide by power-of-2:\n");
    uint32_t n = 256U;
    printf("  %-12s  %-6s  %-10s\n", "n", "Shift", "n >> shift");
    printf("  %-12s  %-6s  %-10s\n", "---", "------", "----------");
    for (shift = 0U; shift <= 8U; ++shift) {
        printf("  %-12u  >> %-3u  %-10u\n", n, shift, n >> shift);
    }

    /* Bitmask generation with BIT_MASK macro */
    printf("\n  BIT_MASK(n) - generates n consecutive 1-bits:\n");
    for (uint8_t w = 1U; w <= 8U; ++w) {
        char binstr[40];
        uint32_t mask = BIT_MASK(w);
        uint_to_binstr(mask, binstr, 8);
        printf("  BIT_MASK(%u) = 0b%s  (0x%02X)\n", w, binstr, mask);
    }
}
