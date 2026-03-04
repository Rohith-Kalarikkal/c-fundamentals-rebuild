/**
 * @file    include/struct_variants.h
 * @brief   Struct type definitions used across padding / sizeof experiments.
 *
 * Design intent
 * -------------
 * Every struct family below demonstrates one concrete aspect of how the
 * compiler inserts padding to satisfy alignment requirements mandated by the
 * ABI (System V AMD64 ABI on Linux x86-64).
 *
 * Golden rule: the alignment of a struct equals the alignment of its
 * largest scalar member.  The compiler inserts:
 *   1. INTERNAL padding  – between members so each member sits at a naturally
 *                          aligned address.
 *   2. TRAILING padding  – after the last member so that sizeof(T) is a
 *                          multiple of alignof(T).  This ensures correctness
 *                          when T is used inside arrays (T arr[N]).
 *
 * Coding standard notes
 * ---------------------
 *  - All types are typedef'd with a _t suffix per MISRA-C / CERT-C convention.
 *  - Include guards use the full path capitalised.
 *  - Packed structs are marked clearly. Avoid them unless interacting with
 *    hardware registers or wire protocols.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef INCLUDE_STRUCT_VARIANTS_H
#define INCLUDE_STRUCT_VARIANTS_H

#include <stdint.h>   /* uint8_t, uint16_t, uint32_t, uint64_t             */
#include <stdbool.h>  /* bool                                              */

/* ------------------------------------------------
 * Naive ("bad") layout — maximises wasted padding
 * ------------------------------------------------
 *
 * Member order:  char(1) | [3 pad] | int(4) | char(1) | [7 pad] | double(8)
 * Expected size: 1 + 3 + 4 + 1 + 7 + 8 = 24 bytes  (16 bytes wasted!)
 */
typedef struct {
    char    c1;       /* offset  0 - 1 byte                                */
                      /* offset  1 - 3 bytes implicit padding              */
    int32_t i;        /* offset  4 - 4 bytes                               */
    char    c2;       /* offset  8 - 1 byte                                */
                      /* offset  9 - 7 bytes implicit padding              */
    double  d;        /* offset 16 - 8 bytes  (requires 8-byte alignment)  */
                      /* total  24 bytes                                   */
} naive_bad_t;

/* ----------------------------------------------
 * Optimised ("good") layout — minimises padding
 * ----------------------------------------------
 *
 * Rule: order members from largest alignment to smallest.
 *
 * Member order:  double(8) | int(4) | char(1) | char(1) | [2 pad]
 * Expected size: 8 + 4 + 1 + 1 + 2 = 16 bytes  (only 2 bytes wasted)
 */
typedef struct {
    double  d;        /* offset  0 - 8 bytes                               */
    int32_t i;        /* offset  8 - 4 bytes                               */
    char    c1;       /* offset 12 - 1 byte                                */
    char    c2;       /* offset 13 - 1 byte                                */
                      /* offset 14 - 2 bytes trailing padding              */
                      /* total  16 bytes                                   */
} naive_good_t;

/* ----------------------------
 * Mixed-width integer fields
 * ----------------------------
 *
 * Demonstrates padding between different-width stdint types.
 *
 * Layout:
 *   uint8(1) | [1 pad] | uint16(2) | uint32(4) | uint8(1) | [7 pad] | uint64(8)
 * Expected size: 24 bytes
 */
typedef struct {
    uint8_t  a;       /* offset  0                                         */
                      /* offset  1 — 1 byte padding (align uint16 to 2)   */
    uint16_t b;       /* offset  2                                         */
    uint32_t c;       /* offset  4                                         */
    uint8_t  d;       /* offset  8                                         */
                      /* offset  9 — 7 bytes padding (align uint64 to 8)  */
    uint64_t e;       /* offset 16                                         */
                      /* total  24 bytes                                   */
} mixed_bad_t;

/* Reordered: largest first → 8 + 4 + 2 + 1 + 1 = 16 bytes, zero padding  */
typedef struct {
    uint64_t e;       /* offset  0                                         */
    uint32_t c;       /* offset  8                                         */
    uint16_t b;       /* offset 12                                         */
    uint8_t  a;       /* offset 14                                         */
    uint8_t  d;       /* offset 15                                         */
                      /* total  16 bytes — no padding at all!              */
} mixed_good_t;

/* -----------------------------------
 *       Nested structs
 * -----------------------------------
 *
 * The outer struct must honour the alignment of the largest member
 * inside any nested struct.  Nesting does NOT hide padding.
 */
typedef struct {
    uint8_t  x;
    uint8_t  y;
} point2d_u8_t;       /* size 2, align 1                                   */

typedef struct {
    float    x;
    float    y;
} point2d_f32_t;      /* size 8, align 4                                   */

typedef struct {
    uint8_t      id;          /* offset  0 - 1 byte                        */
                              /* offset  1 - 3 bytes padding               */
    point2d_f32_t origin;     /* offset  4 - 8 bytes (align 4)             */
    point2d_u8_t  tag;        /* offset 12 - 2 bytes                       */
                              /* offset 14 - 2 bytes trailing padding      */
                              /* total  16 bytes                           */
} nested_shape_t;

/* -------------------------
 *       Bit-fields
 * -------------------------
 *
 * Bit-fields pack flags into the underlying storage unit (here uint8_t).
 * The struct still obeys alignment rules; storage units are NOT split
 * across member boundaries by default.
 */
typedef struct {
    uint8_t  ready   : 1;   /* bit 0                                       */
    uint8_t  error   : 1;   /* bit 1                                       */
    uint8_t  mode    : 3;   /* bits 2-4  (0-7 modes)                       */
    uint8_t  channel : 3;   /* bits 5-7  (0-7 channels)                   */
} hw_flags_t;               /* size 1, align 1                             */

/* Mix of bit-field and normal member */
typedef struct {
    uint32_t address;       /* offset 0 - 4 bytes                          */
    uint8_t  valid    : 1;
    uint8_t  writable : 1;
    uint8_t  cached   : 1;
    uint8_t  reserved : 5;  /* offset 4 - 1 byte (bit-packed)              */
                            /* offset 5 - 3 bytes trailing padding         */
                            /* total  8 bytes                              */
} mmu_entry_t;

/* ------------------------------------------------
 * __attribute__((packed))  [GCC/Clang extension]
 * ------------------------------------------------
 */
typedef struct __attribute__((packed)) {
    char    c1;       /* offset  0 - 1 byte                                */
    int32_t i;        /* offset  1 - 4 bytes  (UNALIGNED!)                 */
    char    c2;       /* offset  5 - 1 byte                                */
    double  d;        /* offset  6 - 8 bytes  (UNALIGNED!)                 */
                      /* total  14 bytes — no padding, but unsafe          */
} packed_t;

/* -------------------------------------------------
 * Cache-line awareness (64-byte lines on x86-64)
 * -------------------------------------------------
 */
#define CACHE_LINE_SIZE  64U

typedef struct {
    /* --- hot path (first 64 bytes = one cache line) ---------------------*/
    uint32_t counter;         /* frequently incremented                    */
    uint32_t flags;           /* read on every ISR entry                   */
    volatile uint32_t status; /* memory-mapped register shadow             */
    uint8_t  _hot_pad[52];    /* explicit pad to fill the cache line       */

    /* --- cold path (second cache line)---------------------------------- */
    char     name[32];        /* set once at init                          */
    uint32_t version;
    uint32_t crc;
} __attribute__((aligned(CACHE_LINE_SIZE))) device_ctx_t;

/* -------------------------------------
 * Flexible array member 
 * =------------------------------------
 *
 * The flexible array member must be the last member.
 * sizeof(flex_packet_t) does NOT include the array storage.
 */
typedef struct {
    uint32_t length;          /* number of valid bytes in payload[]        */
    uint16_t checksum;
    uint8_t  type;
                              /* 1 byte trailing padding before payload    */
    uint8_t  payload[];       /* FAM - allocated dynamically               */
} flex_packet_t;

#endif /* INCLUDE_STRUCT_VARIANTS_H */
