/**
 * @file    src/struct_variants.c
 * @brief   Sizeof experiments, array layout verification, and a systematic
 *          comparison table of all struct variants defined in struct_variants.h
 *
 * Key learning points
 * --------------------
 *  1. sizeof(T[N]) == N * sizeof(T)  always holds.  Trailing padding in T
 *     ensures correct spacing in arrays — this is why trailing padding exists.
 *
 *  2. Pointer arithmetic on T* advances by sizeof(T) bytes - padding included.
 *
 *  3. memcpy / memset operate on sizeof(T) bytes - padding bytes are copied
 *     but their value is unspecified (do NOT use memcmp on padded structs).
 *
 *  4. Embedded register maps must match hardware exactly - use packed structs
 *     or explicitly-sized members with no padding (verify with ASSERT_SIZE).
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "struct_variants.h"
#include "alignment.h"

#include <stdio.h>
#include <string.h>   /* memset, memcpy                                     */
#include <stdlib.h>   /* malloc, free                                       */
#include <stddef.h>
#include <stdalign.h>

/* --- Compile-time ABI regression guards--------------------------------- */
/*
 * Add ASSERT_SIZE for any struct whose layout must never change
 * (e.g. hardware register maps, on-wire protocol headers, EEPROM images).
 * Comment out if running on a 32-bit target where sizeof(long) == 4.
 */
#if defined(__x86_64__) || defined(__aarch64__)
    ASSERT_SIZE(hw_flags_t,  1);
    ASSERT_SIZE(mixed_good_t, 16);
#endif

/* --- Helper ----------------------------------------------------------- */

static void print_banner2(const char *title)
{
    printf("\n");
    printf("-----------------------------------------------------------\n");
    printf("   %-56s   \n", title);
    printf("-----------------------------------------------------------\n");
}

/* --- Summary comparison table -------------------------------------- */

static void summary_table(void)
{
    print_banner2("STRUCT VARIANT SUMMARY TABLE");

    printf("\n  %-26s  %8s  %8s  %8s\n",
           "Type", "sizeof", "alignof", "wasted");
    printf("  %-26s  %8s  %8s  %8s\n",
           "---------------------------", "------", "------", "------");

#define ROW_S(T, raw_sum) \
    printf("  %-26s  %8zu  %8zu  %8zu\n", \
           #T, sizeof(T), alignof(T), calc_padding(sizeof(T), (raw_sum)))

    ROW_S(naive_bad_t,
          sizeof(char)+sizeof(int32_t)+sizeof(char)+sizeof(double));
    ROW_S(naive_good_t,
          sizeof(double)+sizeof(int32_t)+sizeof(char)+sizeof(char));
    ROW_S(mixed_bad_t,
          sizeof(uint8_t)+sizeof(uint16_t)+sizeof(uint32_t)+
          sizeof(uint8_t)+sizeof(uint64_t));
    ROW_S(mixed_good_t,
          sizeof(uint64_t)+sizeof(uint32_t)+sizeof(uint16_t)+
          sizeof(uint8_t)+sizeof(uint8_t));
    ROW_S(nested_shape_t,
          sizeof(uint8_t)+sizeof(point2d_f32_t)+sizeof(point2d_u8_t));
    ROW_S(hw_flags_t,   1);
    ROW_S(mmu_entry_t,  sizeof(uint32_t)+1);
    ROW_S(packed_t,
          sizeof(char)+sizeof(int32_t)+sizeof(char)+sizeof(double));
    ROW_S(device_ctx_t,
          sizeof(uint32_t)*3 + 52 + 32 + sizeof(uint32_t)*2);
    ROW_S(flex_packet_t,
          sizeof(uint32_t)+sizeof(uint16_t)+sizeof(uint8_t));
#undef ROW_S
}

/* --- Array layout proof --------------------------------------- n */

static void array_layout(void)
{
    print_banner2("ARRAY LAYOUT - sizeof(T[N]) == N * sizeof(T)");

    naive_bad_t  arr_bad[3];
    naive_good_t arr_good[3];

    printf("\n  naive_bad_t  arr[3]:\n");
    printf("    sizeof(arr)    = %zu\n", sizeof(arr_bad));
    printf("    3*sizeof(T)    = %zu\n", 3 * sizeof(naive_bad_t));
    for (int i = 0; i < 3; i++) {
        printf("    &arr[%d]       = %p  (delta from [0]: %td bytes)\n",
               i, (void *)&arr_bad[i],
               (char *)&arr_bad[i] - (char *)&arr_bad[0]);
    }

    printf("\n  naive_good_t arr[3]:\n");
    printf("    sizeof(arr)    = %zu\n", sizeof(arr_good));
    printf("    3*sizeof(T)    = %zu\n", 3 * sizeof(naive_good_t));
    for (int i = 0; i < 3; i++) {
        printf("    &arr[%d]       = %p  (delta from [0]: %td bytes)\n",
               i, (void *)&arr_good[i],
               (char *)&arr_good[i] - (char *)&arr_good[0]);
    }

    printf("\n  -> Trailing padding in naive_bad_t ensures each element is\n");
    printf("    placed at an 8-byte aligned address (required by double).\n");
}

/* --- memcmp pitfall with padded structs ---------------------------------- */

static void memcmp_pitfall(void)
{
    print_banner2("MEMCMP PITFALL - Padding bytes are UNSPECIFIED");

    naive_bad_t a, b;

    /*
     * Zero the entire struct first (including padding bytes).
     * Without this, padding bytes contain stack garbage - memcmp will report
     * "not equal" even when all named fields are identical.
     */
    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));

    a.c1 = 'X';  a.i = 100;  a.c2 = 'Y';  a.d = 3.14;
    b.c1 = 'X';  b.i = 100;  b.c2 = 'Y';  b.d = 3.14;

    printf("\n  Two naive_bad_t structs with identical fields:\n");
    printf("    memset to 0 first, then assign fields.\n");
    printf("    memcmp result = %d  (0 = equal - safe because we zeroed)\n",
           memcmp(&a, &b, sizeof(a)));

    /* Now show the UNSAFE version */
    naive_bad_t c, d;
    /* Intentionally NOT zeroing - padding bytes are garbage from the stack */
    c.c1 = 'X';  c.i = 100;  c.c2 = 'Y';  c.d = 3.14;
    d.c1 = 'X';  d.i = 100;  d.c2 = 'Y';  d.d = 3.14;

    printf("\n  Same fields but WITHOUT prior memset (stack garbage in padding):\n");
    printf("    memcmp result = %d  (may be non-zero - fields equal but padding differs!)\n",
           memcmp(&c, &d, sizeof(c)));
    printf("\n  NEVER use memcmp on padded structs unless you called\n");
    printf("    memset(&s, 0, sizeof(s)) before populating all fields.\n");
}

/* --- Flexible array member allocation -------------------------------------- */

static void fam_allocation(void)
{
    print_banner2("FLEXIBLE ARRAY MEMBER - Dynamic Allocation");

    const uint32_t payload_len = 16U;
    const size_t   alloc_size  = sizeof(flex_packet_t) + payload_len;

    flex_packet_t *pkt = (flex_packet_t *)malloc(alloc_size);
    if (pkt == NULL) {
        fprintf(stderr, "  [ERROR] malloc failed\n");
        return;
    }

    pkt->length   = payload_len;
    pkt->checksum = 0xABCDU;
    pkt->type     = 0x01U;

    for (uint32_t i = 0; i < payload_len; i++) {
        pkt->payload[i] = (uint8_t)(i * 2U);
    }

    printf("\n  Allocated %zu bytes for flex_packet_t + %u payload bytes.\n",
           alloc_size, payload_len);
    printf("  sizeof(flex_packet_t) = %zu  (header only, no payload)\n",
           sizeof(flex_packet_t));
    printf("  pkt->length    = %u\n", pkt->length);
    printf("  pkt->checksum  = 0x%04X\n", pkt->checksum);
    printf("  pkt->type      = 0x%02X\n", pkt->type);
    printf("  pkt->payload   = { ");
    for (uint32_t i = 0; i < payload_len; i++) {
        printf("%u ", pkt->payload[i]);
    }
    printf("}\n");

    free(pkt);
    pkt = NULL;
}

/* --- Struct initialisation styles ------------------------------------- */

static void initialisation_styles(void)
{
    print_banner2("STRUCT INITIALISATION STYLES (C99 designated initialisers)");

    /* C99 designated initialisers - preferred: explicit, order-independent */
    const naive_good_t s1 = {
        .d  = 2.71828,
        .i  = -7,
        .c1 = 'A',
        .c2 = 'B',
    };

    /* Positional - fragile: breaks silently if members are reordered       */
    const naive_good_t s2 = { 2.71828, -7, 'A', 'B' };

    /* Zero initialiser - zeroes ALL bytes including padding  */
    const naive_good_t s3 = {0};

    printf("\n  Designated:  d=%.5f  i=%d  c1='%c'  c2='%c'\n",
           s1.d, s1.i, s1.c1, s1.c2);
    printf("  Positional:  d=%.5f  i=%d  c1='%c'  c2='%c'\n",
           s2.d, s2.i, s2.c1, s2.c2);
    printf("  Zero-init:   d=%.5f  i=%d  c1='%c'  c2='%c'\n",
           s3.d, s3.i, s3.c1, s3.c2);

    printf("\n  Use designated initialisers in production code.\n");
    printf("   {0} zero-init is portable and zeroes padding bytes too.\n");
}

/* --- Public entry point ------------------------------------------------ */

/**
 * @brief  Run all struct variant sizeof experiments.
 *         Called from main().
 */
void struct_variants_run_all(void)
{
    summary_table();
    array_layout();
    memcmp_pitfall();
    fam_allocation();
    initialisation_styles();
}
