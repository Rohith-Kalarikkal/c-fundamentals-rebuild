# Struct Padding & Memory Layout

## Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Topics Covered](#topics-covered)
- [Key Concepts](#key-concepts)
- [Demonstrations](#demonstrations)
- [Build Instructions](#build-instructions)
- [Running the Program](#running-the-program)
- [Expected Output](#expected-output)
- [Author](#author)

---

## Overview

When writing C for embedded systems, drivers, or performance-critical applications, understanding how the compiler lays out structs in memory is not optional — it directly impacts:

- **RAM usage** — padding can silently double the size of a struct
- **Cache performance** — misaligned hot fields cause unnecessary cache-line fetches
- **Hardware correctness** — register maps and wire protocols must match byte-for-byte
- **Portability** — packed structs that work on x86-64 can trigger hard faults on ARM Cortex-M0

This project provides **runnable, heavily commented C code** that makes every one of these effects visible at runtime using `sizeof`, `offsetof`, `alignof`, and live address inspection.

---

## Project Structure

```
struct_memory_layout/
├── README.md
├── Makefile
├── include/
│   ├── alignment.h        – alignment / padding helpers & macros
│   ├── memory_sections.h  – .text / .data / .bss section declarations
│   └── struct_variants.h  – all struct type definitions
├── src/
│   ├── main.c             – entry point
│   ├── alignment.c        – alignment & padding analysis
│   ├── memory_sections.c  – section placement
│   └── struct_variants.c  – sizeof / layout experiments
└── docs/
    └── notes.md           – theory notes & key takeaways
```

---

## Topics Covered

| Topic | File |
|-------|------|
| Internal padding between struct members           | `include/struct_variants.h`, `src/alignment.c` |
| Trailing padding & array correctness              | `src/struct_variants.c` |
| Member ordering optimisation (largest -> smallest) | `include/struct_variants.h` |
| Nested struct alignment propagation               | `include/struct_variants.h` |
| Bit-fields & storage unit packing                 | `include/struct_variants.h`, `src/alignment.c` |
| `__attribute__((packed))` - use cases & dangers   | `include/struct_variants.h` |
| Cache-line-aware struct layout                    | `include/struct_variants.h` |
| Flexible array members                          | `include/struct_variants.h`,`src/struct_variants.c`|
| `.text` / `.rodata` / `.data` / `.bss` / stack    | `src/memory_sections.c` |
| `memcmp` pitfall with unzeroed padding            | `src/struct_variants.c` |
| Compile-time ABI regression guards (`_Static_assert`) | `include/alignment.h` |

---

## Key Concepts

### Alignment & Padding Rules (System V AMD64 ABI)

The compiler inserts two kinds of padding automatically:

**Internal padding** — inserted *before* a member to align it to a multiple of its own size.

**Trailing padding** — appended *after* the last member so that `sizeof(T)` is a multiple of `alignof(T)`. This guarantees correctness when `T` is used in arrays (`T arr[N]`).

```
Struct alignment = max(alignof(member) for all members)
```

### Member Ordering — the Golden Rule

Sort members from **largest alignment to smallest** to minimise wasted bytes:

```c
/* BAD  — 24 bytes, 10 wasted */
typedef struct {
    char    c1;    /* offset  0 */
                   /* +3 padding */
    int32_t i;     /* offset  4 */
    char    c2;    /* offset  8 */
                   /* +7 padding */
    double  d;     /* offset 16 */
} naive_bad_t;

/* GOOD — 16 bytes, 2 wasted */
typedef struct {
    double  d;     /* offset  0 */
    int32_t i;     /* offset  8 */
    char    c1;    /* offset 12 */
    char    c2;    /* offset 13 */
                   /* +2 trailing padding */
} naive_good_t;
```

For an array of 1 million structs: **8 bytes × 1,000,000 = 8 MB saved** by reordering alone.

### ELF Sections

```
 ---------------------------------------------------------
|                      ELF binary on disk                 |
|  .text    — machine code         (read-only, execute)   |
|  .rodata  — const globals/strs   (read-only)            |
|  .data    — initialised globals  (read-write, in file)  |
|  .bss     — zero-init globals    (read-write, NO space) |
 ---------------------------------------------------------
```

`.bss` is critical on embedded targets — large zero-init buffers consume **no flash storage**. Only data with a non-zero initial value consumes flash via `.data`.

### `__attribute__((packed))` Warning

Packed structs eliminate padding but produce **unaligned memory accesses**:

| Architecture | Effect of unaligned access |
|---|---|
| x86-64 | Performance penalty (possible split cache-line) |
| ARM Cortex-M0/M1 | **Hardware fault (UsageFault)** |
| MIPS / RISC-V (no-C extension) | **Hardware fault** |

Use packed structs **only** for hardware register maps, on-wire protocol frames, or EEPROM layouts — and always guard with `ASSERT_SIZE`.

---

## Demonstrations

The program runs **11 self-contained demos** across three modules:

| # | Function | Key Result |
|---|------|-----------|
| 1 | Scalar `sizeof`/`alignof` reference table | `long double` = 16 bytes, `void*` = 8 bytes on x86-64 |
| 2 | `naive_bad_t` vs `naive_good_t` | 24 → 16 bytes — **8 bytes saved by reordering** |
| 3 | Mixed-width integer fields | `mixed_good_t` = 16 bytes with **zero wasted padding** |
| 4 | Nested structs | Inner struct alignment propagates to outer struct |
| 5 | Bit-fields | 8 flags packed into 1 byte; `offsetof` forbidden on bit-fields |
| 6 | `__attribute__((packed))` | 14 bytes, `int` at offset 1 — unaligned, fault risk on ARM |
| 7 | Cache-line-aware struct | Hot fields in cache line 0 (offsets 0–63), cold in line 1 |
| 8 | Flexible array member | `sizeof(flex_packet_t)` = 8 — FAM storage not counted |
| 9 | Array layout proof | `sizeof(T[3])` == `3 × sizeof(T)` confirmed with live addresses |
| 10 | `memcmp` pitfall | Without `memset`, padding bytes cause false inequality |
| 11 | ELF section placement | `.rodata`, `.data`, `.bss`, stack addresses printed live |

---

## Build Instructions

### Prerequisites

| GCC | 10.x or later |
| GNU Make | 4.x or later |

---

### Using Make

```bash
make          # compile all source files -> produces ./pointers_demo
make run      # compile (if needed) then run immediately
make sections # updated sections target
make clean    # remove all
make help     # List all the make instructions
```

---

## Running the Program

```bash
# Full build + run in one step:
make run

# Or run the already-compiled binary directly:
./build/struct.exe
```

---

## Expected Output

```
------------------------------------------------------------
            Struct Padding & Memory Layout
------------------------------------------------------------

------------------------------------------------------------
   sizeof / alignof Scalar Reference Table
------------------------------------------------------------

  Type                          sizeof   alignof
  ----------------------------  ------  --------
  char                               1         1
  signed char                        1         1
  unsigned char                      1         1
  short                              2         2
  int                                4         4
  long                               4         4
  long long                          8         8
  float                              4         4
  double                             8         8
  long double                       16        16
  void *                             8         8
  uint8_t                            1         1
  uint16_t                           2         2
  uint32_t                           4         4
  uint64_t                           8         8
  bool                               1         1

------------------------------------------------------------
   Naive Bad vs Optimised Good Layout
------------------------------------------------------------

  [naive_bad_t]  (worst-case member ordering)
  sizeof(naive_bad_t)            ) = 24  alignof = 8
------------------------------------------------------------
  offsetof(naive_bad_t         , c1          ) =  0 bytes
  offsetof(naive_bad_t         , i           ) =  4 bytes
  offsetof(naive_bad_t         , c2          ) =  8 bytes
  offsetof(naive_bad_t         , d           ) = 16 bytes
------------------------------------------------------------
  Raw member sum  = 14 bytes
  sizeof result   = 24 bytes
  Wasted padding  = 10 bytes

  [naive_good_t]  (members ordered largest -> smallest)
  sizeof(naive_good_t)           ) = 16  alignof = 8
------------------------------------------------------------
  offsetof(naive_good_t        , d           ) =  0 bytes
  offsetof(naive_good_t        , i           ) =  8 bytes
  offsetof(naive_good_t        , c1          ) = 12 bytes
  offsetof(naive_good_t        , c2          ) = 13 bytes
------------------------------------------------------------
  Raw member sum  = 14 bytes
  sizeof result   = 16 bytes
  Wasted padding  = 2 bytes

  Saving: 8 bytes per instance just by reordering!

------------------------------------------------------------
   Mixed-Width Integer Fields
------------------------------------------------------------

  [mixed_bad_t]
  sizeof(mixed_bad_t)            ) = 24  alignof = 8
------------------------------------------------------------
  offsetof(mixed_bad_t         , a           ) =  0 bytes
  offsetof(mixed_bad_t         , b           ) =  2 bytes
  offsetof(mixed_bad_t         , c           ) =  4 bytes
  offsetof(mixed_bad_t         , d           ) =  8 bytes
  offsetof(mixed_bad_t         , e           ) = 16 bytes
------------------------------------------------------------
  Wasted = 8 bytes

  [mixed_good_t]  (uint64 -> uint32 -> uint16 -> uint8 -> uint8)
  sizeof(mixed_good_t)           ) = 16  alignof = 8
------------------------------------------------------------
  offsetof(mixed_good_t        , e           ) =  0 bytes
  offsetof(mixed_good_t        , c           ) =  8 bytes
  offsetof(mixed_good_t        , b           ) = 12 bytes
  offsetof(mixed_good_t        , a           ) = 14 bytes
  offsetof(mixed_good_t        , d           ) = 15 bytes
------------------------------------------------------------
  Wasted = 0 bytes

------------------------------------------------------------
    Nested Structs
------------------------------------------------------------

  [point2d_u8_t]
  sizeof(point2d_u8_t)           ) =  2  alignof = 1

  [point2d_f32_t]
  sizeof(point2d_f32_t)          ) =  8  alignof = 4

  [nested_shape_t]
  sizeof(nested_shape_t)         ) = 16  alignof = 4
------------------------------------------------------------
  offsetof(nested_shape_t      , id          ) =  0 bytes
  offsetof(nested_shape_t      , origin      ) =  4 bytes
  offsetof(nested_shape_t      , tag         ) = 12 bytes
------------------------------------------------------------
  Note: 3 bytes padding after 'id' because point2d_f32_t requires 4-byte alignment.
  Note: 2 bytes trailing padding added by compiler.

------------------------------------------------------------
   Bit-Fields
------------------------------------------------------------

  [hw_flags_t]  - 8 flags packed into 1 byte
  sizeof(hw_flags_t)             ) =  1  alignof = 1
  (All 8 bits used: ready[1] error[1] mode[3] channel[3])

  [mmu_entry_t]
  sizeof(mmu_entry_t)            ) =  8  alignof = 4
------------------------------------------------------------
  offsetof(mmu_entry_t         , address     ) =  0 bytes
  offsetof(mmu_entry_t, valid/writable/cached) = N/A (bit-field: C standard forbids offsetof)
------------------------------------------------------------
  Note: 3 bytes trailing padding after the bit-field byte.

  hw_flags_t - raw byte value: 0x75
    ready=1  error=0  mode=5  channel=3

------------------------------------------------------------
   __attribute__((packed))  [USE WITH CAUTION]
------------------------------------------------------------

  [naive_bad_t]   (normal, with padding)
  sizeof(naive_bad_t)            ) = 24  alignof = 8

  [packed_t] (same members, packed - unaligned accesses!)
  sizeof(packed_t)               ) = 14  alignof = 1
------------------------------------------------------------
  offsetof(packed_t            , c1          ) =  0 bytes
  offsetof(packed_t            , i           ) =  1 bytes
  offsetof(packed_t            , c2          ) =  5 bytes
  offsetof(packed_t            , d           ) =  6 bytes
------------------------------------------------------------
  i sits at offset 1 (not a multiple of 4).
  d sits at offset 6 (not a multiple of 8).
  On ARM Cortex-M0/M1/MIPS strict-align ΓåÆ hardware fault!
  Safe on x86-64, but incurs a split-cache-line penalty.

------------------------------------------------------------
   Cache-Line Alignment (64 bytes)
------------------------------------------------------------

  [device_ctx_t]  aligned(64)
  sizeof(device_ctx_t)           ) = 128  alignof = 64
------------------------------------------------------------
  offsetof(device_ctx_t        , counter     ) =  0 bytes
  offsetof(device_ctx_t        , flags       ) =  4 bytes
  offsetof(device_ctx_t        , status      ) =  8 bytes
  offsetof(device_ctx_t        , name        ) = 64 bytes
  offsetof(device_ctx_t        , version     ) = 96 bytes
  offsetof(device_ctx_t        , crc         ) = 100 bytes
------------------------------------------------------------
  Hot fields (counter, flags, status) share cache line 0 [offsets 0-63].
  Cold fields (name, version, crc) live in cache line 1 [offsets 64+].
  alignof = 64 -> guaranteed cache-line alignment.

------------------------------------------------------------
   Flexible Array Member (C99 FAM)
------------------------------------------------------------

  [flex_packet_t]
  sizeof(flex_packet_t)          ) =  8  alignof = 4
------------------------------------------------------------
  offsetof(flex_packet_t       , length      ) =  0 bytes
  offsetof(flex_packet_t       , checksum    ) =  4 bytes
  offsetof(flex_packet_t       , type        ) =  6 bytes
  offsetof(flex_packet_t       , payload     ) =  7 bytes
------------------------------------------------------------
  sizeof() = 8  (does NOT include payload storage)
  Allocate: malloc(sizeof(flex_packet_t) + N) for N payload bytes.

-----------------------------------------------------------
   STRUCT VARIANT SUMMARY TABLE
-----------------------------------------------------------

  Type                          sizeof   alignof    wasted
  ---------------------------    ------    ------    ------
  naive_bad_t                       24         8        10
  naive_good_t                      16         8         2
  mixed_bad_t                       24         8         8
  mixed_good_t                      16         8         0
  nested_shape_t                    16         4         5
  hw_flags_t                         1         1         0
  mmu_entry_t                        8         4         3
  packed_t                          14         1         0
  device_ctx_t                     128        64        24
  flex_packet_t                      8         4         1

-----------------------------------------------------------
   ARRAY LAYOUT - sizeof(T[N]) == N * sizeof(T)
-----------------------------------------------------------

  naive_bad_t  arr[3]:
    sizeof(arr)    = 72
    3*sizeof(T)    = 72
    &arr[0]       = 000000DBD21FFB10  (delta from [0]: 0 bytes)
    &arr[1]       = 000000DBD21FFB28  (delta from [0]: 24 bytes)
    &arr[2]       = 000000DBD21FFB40  (delta from [0]: 48 bytes)

  naive_good_t arr[3]:
    sizeof(arr)    = 48
    3*sizeof(T)    = 48
    &arr[0]       = 000000DBD21FFAE0  (delta from [0]: 0 bytes)
    &arr[1]       = 000000DBD21FFAF0  (delta from [0]: 16 bytes)
    &arr[2]       = 000000DBD21FFB00  (delta from [0]: 32 bytes)

  -> Trailing padding in naive_bad_t ensures each element is
    placed at an 8-byte aligned address (required by double).

-----------------------------------------------------------
   MEMCMP PITFALL - Padding bytes are UNSPECIFIED
-----------------------------------------------------------

  Two naive_bad_t structs with identical fields:
    memset to 0 first, then assign fields.
    memcmp result = 0  (0 = equal - safe because we zeroed)

  Same fields but WITHOUT prior memset (stack garbage in padding):
    memcmp result = -1  (may be non-zero - fields equal but padding differs!)

  NEVER use memcmp on padded structs unless you called
    memset(&s, 0, sizeof(s)) before populating all fields.

-----------------------------------------------------------
   FLEXIBLE ARRAY MEMBER - Dynamic Allocation
-----------------------------------------------------------

  Allocated 24 bytes for flex_packet_t + 16 payload bytes.
  sizeof(flex_packet_t) = 8  (header only, no payload)
  pkt->length    = 16
  pkt->checksum  = 0xABCD
  pkt->type      = 0x01
  pkt->payload   = { 0 2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 }

-----------------------------------------------------------
   STRUCT INITIALISATION STYLES (C99 designated initialisers)
-----------------------------------------------------------

  Designated:  d=2.71828  i=-7  c1='A'  c2='B'
  Positional:  d=2.71828  i=-7  c1='A'  c2='B'
  Zero-init:   d=0.00000  i=0  c1=''  c2=''

  Use designated initialisers in production code.
   {0} zero-init is portable and zeroes padding bytes too.

----------------------------------------------------------------
                ELF MEMORY SECTIONS
----------------------------------------------------------------

  Variable                      Address             Section   Notes
  ----------------------------  ------------------  --------  -----
  g_magic_number                00007FF6247A9000  .rodata   const, read-only, value=0xDEADBEEF
  k_build_tag (ptr)             00007FF6247A9008  .rodata   const char* const
  *k_build_tag (str)            00007FF6247A6C00  .rodata   string literal
  g_initialized_counter         00007FF6247A5000  .data     non-const, init=42
  s_retry_limit                 00007FF6247A5004  .data     static, init=3
  g_zero_counter                00007FF6247AC040  .bss      non-const, zero-init (no file space)
  g_rx_buffer[0]                00007FF6247AC060  .bss      256-byte array, zero-init
  s_error_count                 00007FF6247AC160  .bss      static, zero-init

  .bss zero-init verification:
    g_zero_counter  = 0  (expected 0)
    s_error_count   = 0  (expected 0)
    g_rx_buffer[0]  = 0  (expected 0)

  .data mutation demo:
    g_initialized_counter before = 42
    g_initialized_counter after  = 43

  [Stack / automatic variables - NOT in any ELF section]
  stack_u8                      000000DBD21FFB4F  stack     automatic, 1 byte
  stack_u32                     000000DBD21FFB48  stack     automatic, 4 bytes
  stack_u64                     000000DBD21FFB40  stack     automatic, 8 bytes

  Stack grows downward on x86-64:
  &stack_u64 (000000DBD21FFB40) < &stack_u32 (000000DBD21FFB48) < &stack_u8 (000000DBD21FFB4F)  ->  confirmed

  TIP: Run  'nm --numeric-sort ./struct_demo'  to confirm
       section placement by examining the symbol table.

  TIP: Run  'make sections'  to see raw ELF section sizes.

--------------------------------------------------------------
                         Completed.
--------------------------------------------------------------

```

---

## Author

**Rohith Kalarikkal Ramakrishnan**  
Version: 1.0.0 
Language: C11

---

