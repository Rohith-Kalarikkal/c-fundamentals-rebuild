/**
 * @file    src/alignment.c
 * @brief   Runtime alignment & padding analysis for all struct variants.
 *
 * Each function:
 *  1. Prints sizeof / alignof for the type.
 *  2. Prints byte offsets of every member (offsetof).
 *  3. Shows the raw sum of member sizes so wasted bytes are obvious.
 *  4. Uses compile-time assertions to catch ABI regressions.
 *
 * How to read the output
 * ----------------------
 *  "sizeof(T) = N   alignof = A"
 *       N is total bytes including padding.
 *       A is the minimum address alignment the compiler requires.
 *
 *  "offsetof(T, field) = B"
 *       B is the distance in bytes from the start of T to that member.
 *       Gaps between consecutive offsets reveal inserted padding.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "alignment.h"
#include "struct_variants.h"

#include <stdio.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdint.h>

/* --- Internal helpers ------------------------------------------------------ */

/** Print a section banner for readability in the terminal output.             */
static void print_banner(const char *title)
{
    printf("\n");
    printf("------------------------------------------------------------\n");
    printf("   %-56s  \n", title);
    printf("------------------------------------------------------------\n");
}

static void print_separator(void)
{
    printf("------------------------------------------------------------\n");
}

/* naive_bad_t vs naive_good_t ---------------------------------------------- */

static void naive_layout(void)
{
    print_banner("Naive Bad vs Optimised Good Layout");

    /* --- naive_bad_t --- */
    printf("\n  [naive_bad_t]  (worst-case member ordering)\n");
    PRINT_SIZE_ALIGN(naive_bad_t);
    print_separator();
    PRINT_OFFSET(naive_bad_t, c1);
    PRINT_OFFSET(naive_bad_t, i);
    PRINT_OFFSET(naive_bad_t, c2);
    PRINT_OFFSET(naive_bad_t, d);
    print_separator();

    {
        size_t raw = sizeof(char) + sizeof(int32_t) + sizeof(char) + sizeof(double);
        printf("  Raw member sum  = %zu bytes\n", raw);
        printf("  sizeof result   = %zu bytes\n", sizeof(naive_bad_t));
        printf("  Wasted padding  = %zu bytes\n",
               calc_padding(sizeof(naive_bad_t), raw));
    }

    /* --- naive_good_t --- */
    printf("\n  [naive_good_t]  (members ordered largest -> smallest)\n");
    PRINT_SIZE_ALIGN(naive_good_t);
    print_separator();
    PRINT_OFFSET(naive_good_t, d);
    PRINT_OFFSET(naive_good_t, i);
    PRINT_OFFSET(naive_good_t, c1);
    PRINT_OFFSET(naive_good_t, c2);
    print_separator();

    {
        size_t raw = sizeof(double) + sizeof(int32_t) + sizeof(char) + sizeof(char);
        printf("  Raw member sum  = %zu bytes\n", raw);
        printf("  sizeof result   = %zu bytes\n", sizeof(naive_good_t));
        printf("  Wasted padding  = %zu bytes\n",
               calc_padding(sizeof(naive_good_t), raw));
    }

    printf("\n  Saving: %zu bytes per instance just by reordering!\n",
           sizeof(naive_bad_t) - sizeof(naive_good_t));
}

/* Mixed-width integer fields ------------------------------------------- */

static void mixed_widths(void)
{
    print_banner("Mixed-Width Integer Fields");

    printf("\n  [mixed_bad_t]\n");
    PRINT_SIZE_ALIGN(mixed_bad_t);
    print_separator();
    PRINT_OFFSET(mixed_bad_t, a);
    PRINT_OFFSET(mixed_bad_t, b);
    PRINT_OFFSET(mixed_bad_t, c);
    PRINT_OFFSET(mixed_bad_t, d);
    PRINT_OFFSET(mixed_bad_t, e);
    print_separator();
    printf("  Wasted = %zu bytes\n",
           calc_padding(sizeof(mixed_bad_t),
                        sizeof(uint8_t) + sizeof(uint16_t) +
                        sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint64_t)));

    printf("\n  [mixed_good_t]  (uint64 -> uint32 -> uint16 -> uint8 -> uint8)\n");
    PRINT_SIZE_ALIGN(mixed_good_t);
    print_separator();
    PRINT_OFFSET(mixed_good_t, e);
    PRINT_OFFSET(mixed_good_t, c);
    PRINT_OFFSET(mixed_good_t, b);
    PRINT_OFFSET(mixed_good_t, a);
    PRINT_OFFSET(mixed_good_t, d);
    print_separator();
    printf("  Wasted = %zu bytes\n",
           calc_padding(sizeof(mixed_good_t),
                        sizeof(uint8_t) + sizeof(uint16_t) +
                        sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint64_t)));
}

/* --- Nested structs ------------------------------------------------- */

static void nested_structs(void)
{
    print_banner(" Nested Structs");

    printf("\n  [point2d_u8_t]\n");
    PRINT_SIZE_ALIGN(point2d_u8_t);

    printf("\n  [point2d_f32_t]\n");
    PRINT_SIZE_ALIGN(point2d_f32_t);

    printf("\n  [nested_shape_t]\n");
    PRINT_SIZE_ALIGN(nested_shape_t);
    print_separator();
    PRINT_OFFSET(nested_shape_t, id);
    PRINT_OFFSET(nested_shape_t, origin);
    PRINT_OFFSET(nested_shape_t, tag);
    print_separator();
    printf("  Note: 3 bytes padding after 'id' because point2d_f32_t "
           "requires 4-byte alignment.\n");
    printf("  Note: 2 bytes trailing padding added by compiler.\n");
}

/* --- Bit-fields -------------------------------------------------- */

static void bitfields(void)
{
    print_banner("Bit-Fields");

    printf("\n  [hw_flags_t]  - 8 flags packed into 1 byte\n");
    PRINT_SIZE_ALIGN(hw_flags_t);
    printf("  (All 8 bits used: ready[1] error[1] mode[3] channel[3])\n");

    printf("\n  [mmu_entry_t]\n");
    PRINT_SIZE_ALIGN(mmu_entry_t);
    print_separator();
    PRINT_OFFSET(mmu_entry_t, address);
    /* Bit-field offsets cannot be taken with offsetof — documented here.   */
    printf("  offsetof(mmu_entry_t, valid/writable/cached) = N/A "
           "(bit-field: C standard forbids offsetof)\n");
    print_separator();
    printf("  Note: 3 bytes trailing padding after the bit-field byte.\n");

    /* Demonstrate bit-field read/write */
    hw_flags_t f = {0};
    f.ready   = 1;
    f.mode    = 5;   /* binary 101 */
    f.channel = 3;   /* binary 011 */
    printf("\n  hw_flags_t - raw byte value: 0x%02X\n", *((uint8_t *)&f));
    printf("    ready=%u  error=%u  mode=%u  channel=%u\n",
           f.ready, f.error, f.mode, f.channel);
}

/* --- Packed struct --------------------------------------------------- */

static void packed(void)
{
    print_banner("__attribute__((packed))  [USE WITH CAUTION]");

    printf("\n  [naive_bad_t]   (normal, with padding)\n");
    PRINT_SIZE_ALIGN(naive_bad_t);

    printf("\n  [packed_t] (same members, packed - unaligned accesses!)\n");
    PRINT_SIZE_ALIGN(packed_t);
    print_separator();
    PRINT_OFFSET(packed_t, c1);
    PRINT_OFFSET(packed_t, i);
    PRINT_OFFSET(packed_t, c2);
    PRINT_OFFSET(packed_t, d);
    print_separator();
    printf("  i sits at offset 1 (not a multiple of 4).\n");
    printf("  d sits at offset 6 (not a multiple of 8).\n");
    printf("  On ARM Cortex-M0/M1/MIPS strict-align → hardware fault!\n");
    printf("  Safe on x86-64, but incurs a split-cache-line penalty.\n");
}

/* --- Cache-line-aligned struct -------------------------------------- */

static void cache_line(void)
{
    print_banner("Cache-Line Alignment (64 bytes)");

    printf("\n  [device_ctx_t]  aligned(%d)\n", CACHE_LINE_SIZE);
    PRINT_SIZE_ALIGN(device_ctx_t);
    print_separator();
    PRINT_OFFSET(device_ctx_t, counter);
    PRINT_OFFSET(device_ctx_t, flags);
    PRINT_OFFSET(device_ctx_t, status);
    PRINT_OFFSET(device_ctx_t, name);
    PRINT_OFFSET(device_ctx_t, version);
    PRINT_OFFSET(device_ctx_t, crc);
    print_separator();
    printf("  Hot fields (counter, flags, status) share cache line 0 "
           "[offsets 0-63].\n");
    printf("  Cold fields (name, version, crc) live in cache line 1 "
           "[offsets 64+].\n");
    printf("  alignof = %zu -> guaranteed cache-line alignment.\n",
           alignof(device_ctx_t));
}

/* --- Flexible array member ------------------------------- */

static void flexible_array(void)
{
    print_banner("Flexible Array Member (C99 FAM)");

    printf("\n  [flex_packet_t]\n");
    PRINT_SIZE_ALIGN(flex_packet_t);
    print_separator();
    PRINT_OFFSET(flex_packet_t, length);
    PRINT_OFFSET(flex_packet_t, checksum);
    PRINT_OFFSET(flex_packet_t, type);
    PRINT_OFFSET(flex_packet_t, payload);
    print_separator();
    printf("  sizeof() = %zu  (does NOT include payload storage)\n",
           sizeof(flex_packet_t));
    printf("  Allocate: malloc(sizeof(flex_packet_t) + N) for N payload bytes.\n");
}

/* --- sizeof on common scalar types ----------------------------------------- */

static void scalar_sizes(void)
{
    print_banner("sizeof / alignof Scalar Reference Table");

    printf("\n  %-28s  %6s  %8s\n", "Type", "sizeof", "alignof");
    printf("  %-28s  %6s  %8s\n",
           "----------------------------", "------", "--------");

#define ROW(T) printf("  %-28s  %6zu  %8zu\n", #T, sizeof(T), alignof(T))
    ROW(char);
    ROW(signed char);
    ROW(unsigned char);
    ROW(short);
    ROW(int);
    ROW(long);
    ROW(long long);
    ROW(float);
    ROW(double);
    ROW(long double);
    ROW(void *);
    ROW(uint8_t);
    ROW(uint16_t);
    ROW(uint32_t);
    ROW(uint64_t);
    ROW(bool);
#undef ROW
}

/* --- Public entry point ---------------------------------------- */

/**
 * @brief  Run all alignment & padding codes.
 *         Called from main().
 */
void alignment_run_all(void)
{
    /*
     * Compile-time sanity checks — these fire at compile time, not runtime.
     * If any of these fire on your target, the ABI assumptions are wrong.
     */
    ASSERT_ALIGN(double,   8);
    ASSERT_ALIGN(uint64_t, 8);
    ASSERT_ALIGN(uint32_t, 4);
    ASSERT_ALIGN(uint16_t, 2);
    ASSERT_ALIGN(uint8_t,  1);

    scalar_sizes();
    naive_layout();
    mixed_widths();
    nested_structs();
    bitfields();
    packed();
    cache_line();
    flexible_array();
}
