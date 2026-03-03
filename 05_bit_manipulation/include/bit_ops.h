/**
 * @file    bit_ops.h
 * @brief   Bit manipulation macros and utility declarations.
 *
 * Provides portable, hardware-agnostic macros for common bit-level
 * operations used across embedded firmware. All macros operate on
 * unsigned integer types to avoid undefined behaviour with signed
 * shift operations.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 *
 */

#ifndef BIT_OPS_H
#define BIT_OPS_H

#include <stdint.h>   /* uint8_t, uint16_t, uint32_t              */
#include <stdbool.h>  /* bool, true, false                         */

/* ------------------------------------------------------------------ */
/*  Compiler portability                                               */
/* ------------------------------------------------------------------ */

/** Force inline — falls back gracefully if compiler does not support. */
#if defined(__GNUC__) || defined(__clang__)
    #define INLINE __attribute__((always_inline)) static inline
#else
    #define INLINE static inline
#endif

/* ------------------------------------------------------------------ */
/*  Core bit-manipulation macros                                       */
/* ------------------------------------------------------------------ */

/**
 * @defgroup CORE_BIT_OPS Core Bit Operations
 */

/**
 * @brief  Set a single bit at position BIT in register/variable REG.
 *         Uses bitwise OR: REG = REG | (1 << BIT)
 *
 * @param  REG  Any l-value (variable or dereferenced pointer).
 * @param  BIT  Zero-based bit position (0 = LSB).
 */
#define SET_BIT(REG, BIT)       ((REG) |=  (1UL << (BIT)))

/**
 * @brief  Clear a single bit at position BIT in REG.
 *         Uses AND with inverted mask: REG = REG & ~(1 << BIT)
 *
 * @param  REG  Any l-value.
 * @param  BIT  Zero-based bit position.
 */
#define CLEAR_BIT(REG, BIT)     ((REG) &= ~(1UL << (BIT)))

/**
 * @brief  Toggle (flip) a single bit at position BIT in REG.
 *         Uses XOR: REG = REG ^ (1 << BIT)
 *
 * @param  REG  Any l-value.
 * @param  BIT  Zero-based bit position.
 */
#define TOGGLE_BIT(REG, BIT)    ((REG) ^=  (1UL << (BIT)))

/**
 * @brief  Check whether a bit at position BIT in REG is set.
 *         Returns non-zero (true) if the bit is 1, zero (false) otherwise.
 *
 * @param  REG  Any r-value or l-value.
 * @param  BIT  Zero-based bit position.
 */
#define CHECK_BIT(REG, BIT)     (((REG) >>  (BIT)) & 1UL)

/**
 * @brief  Read the full value of REG (for completeness / named alias).
 */
#define READ_REG(REG)           (REG)

/**
 * @brief  Write a raw value directly to REG.
 *
 * @param  REG  Any l-value.
 * @param  VAL  Value to write.
 */
#define WRITE_REG(REG, VAL)     ((REG) = (VAL))

/* end CORE_BIT_OPS */

/* ------------------------------------------------------------------ */
/*  Multi-bit / field macros                                           */
/* ------------------------------------------------------------------ */

/**
 * @defgroup FIELD_OPS Multi-bit Field Operations
 * @{
 */

/**
 * @brief  Build a bitmask of WIDTH consecutive 1-bits.
 *         Example: BIT_MASK(3) => 0b00000111
 *
 * @param  WIDTH  Number of bits in the mask (1–32).
 */
#define BIT_MASK(WIDTH)         ((1UL << (WIDTH)) - 1UL)

/**
 * @brief  Extract a bit-field of WIDTH bits starting at bit POS.
 *         Example: GET_FIELD(0xF0, 4, 4) => 0x0F
 *
 * @param  REG    Source value.
 * @param  POS    LSB position of the field.
 * @param  WIDTH  Number of bits in the field.
 */
#define GET_FIELD(REG, POS, WIDTH)  (((REG) >> (POS)) & BIT_MASK(WIDTH))

/**
 * @brief  Write a bit-field into REG without disturbing other bits.
 *         Performs: clear existing field, then OR in new value.
 *
 * @param  REG    l-value destination.
 * @param  POS    LSB position of the field.
 * @param  WIDTH  Number of bits in the field.
 * @param  VAL    New value (must fit within WIDTH bits).
 */
#define SET_FIELD(REG, POS, WIDTH, VAL) \
    ((REG) = (((REG) & ~(BIT_MASK(WIDTH) << (POS))) | \
              (((VAL) & BIT_MASK(WIDTH)) << (POS))))

/* end FIELD_OPS */

/* ------------------------------------------------------------------ */
/*  Function declarations (implemented in bit_ops.c)                   */
/* ------------------------------------------------------------------ */

/**
 * @brief  Print a 32-bit value as a formatted binary string.
 *         Output:  0b 1111_0000_1010_1010_0000_0000_0000_0000
 *
 * @param  label  Descriptive string printed before the binary value.
 * @param  value  The 32-bit unsigned integer to display.
 */
void print_binary(const char *label, uint32_t value);

/**
 * @brief  Demonstrate all six bitwise operators with annotated output.
 */
void bitwise_operators(void);

/**
 * @brief  Demonstrate set / clear / toggle / check on a single byte.
 */
void bit_operations(void);

/**
 * @brief  Demonstrate left-shift and right-shift (logical) operators.
 */
void shift_operators(void);

#endif /* BIT_OPS_H */
