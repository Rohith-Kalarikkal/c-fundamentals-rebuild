# Struct Padding & Memory Layout — Theory Notes

Note: This documentation was compiled with the assistance of AI to summarize the technical concepts demonstrated in the source code.
---

## Table of Contents

1. [How Computer Memory Works](#1-how-computer-memory-works)
2. [What is Alignment and Why Does it Exist?](#2-what-is-alignment-and-why-does-it-exist)
3. [What is Padding?](#3-what-is-padding)
4. [The Two Types of Padding](#4-the-two-types-of-padding)
5. [Alignment Rules — System V AMD64 ABI](#5-alignment-rules--system-v-amd64-abi)
6. [Optimising Member Ordering](#6-optimising-member-ordering)
7. [Bit-fields](#7-bit-fields)
8. [`__attribute__((packed))` When and Why](#8-__attribute__packed-when-and-why)
9. [Cache-Line Awareness](#9-cache-line-awareness)
10. [Flexible Array Members (FAM)](#10-flexible-array-members-fam)
11. [ELF Memory Sections Explained](#11-elf-memory-sections-explained)
12. [The Stack and the Heap](#12-the-stack-and-the-heap)
13. [Common Pitfalls](#13-common-pitfalls)
14. [Compile-Time Safety with `_Static_assert`](#14-compile-time-safety-with-_static_assert)
15. [Useful Toolchain Commands](#15-useful-toolchain-commands)
16. [Quick Reference Cheat Sheet](#16-quick-reference-cheat-sheet)

---

## 1. How Computer Memory Works

Before understanding padding, you need a mental model of memory.

Think of RAM as a very long street of numbered houses. Each house holds
exactly **1 byte**. The house number is the **memory address**.

```
Address:  0x1000  0x1001  0x1002  0x1003  0x1004  0x1005  0x1006  0x1007
          ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
Data:     │  A  │  B  │  C  │  D  │  E  │  F  │  G  │  H  │
          └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
```

A CPU does not read memory one byte at a time. It reads memory in **chunks**
called **words** (typically 4 or 8 bytes wide on modern CPUs). This is far
more efficient — one memory bus transaction fetches 8 bytes instead of 8
separate transactions fetching 1 byte each.

The catch: **the CPU is most efficient when a chunk starts at an address that
is a multiple of the chunk size.**

- A 4-byte `int` is best placed at address `0x1000`, `0x1004`, `0x1008` ...
- An 8-byte `double` is best placed at address `0x1000`, `0x1008`, `0x1010` ...

These preferred addresses are called **naturally aligned** addresses.

---

## 2. What is Alignment and Why Does it Exist?

**Alignment** is a constraint on where in memory a variable is allowed to live.

A type with alignment `N` must start at an address that is a multiple of `N`.

```
alignof(char)   = 1  → can start at any address (0, 1, 2, 3, ...)
alignof(short)  = 2  → must start at even addresses (0, 2, 4, 6, ...)
alignof(int)    = 4  → must start at multiples of 4 (0, 4, 8, 12, ...)
alignof(double) = 8  → must start at multiples of 8 (0, 8, 16, 24, ...)
```

### What happens when alignment is violated?

**On x86-64 (your PC/laptop):** The CPU handles it in hardware but at a cost.
It fetches two cache lines instead of one and stitches the bytes together. This
is invisible but slower, especially in loops.

```
Address:  0x1006
          ┌─────┬─────┬─────┬─────┐
int at    │byte1│byte2│byte3│byte4│   ← split across two 8-byte cache lines!
0x1006:   └─────┴─────┴─────┴─────┘
           cache line 0 boundary: 0x1000–0x1007
                                   cache line 1 boundary: 0x1008–0x100F
```

**On ARM Cortex-M0, MIPS, RISC-V (microcontrollers):** There is NO hardware
support for unaligned access. The CPU raises a **hardware fault** (UsageFault
on Cortex-M). Your program crashes.

**With DMA (Direct Memory Access):** DMA controllers require aligned buffers.
Passing an unaligned buffer to a DMA peripheral causes **silent data corruption**
— one of the hardest bugs to find in embedded firmware.

---

## 3. What is Padding?

When you write a struct, you choose the members. The **compiler** chooses where
each member lives inside the struct to satisfy alignment requirements.

If a member cannot start at the very next available byte (because that address
wouldn't be aligned), the compiler inserts **invisible filler bytes** called
**padding**. You never write these bytes — they appear automatically.

```c
struct Example {
    char   a;   // you wrote 1 byte
    // ??? bytes inserted here by compiler
    int    b;   // you wrote 4 bytes
};
```

To find out how much padding was inserted, you use:
- `sizeof(struct Example)` — total size including padding
- `offsetof(struct Example, b)` — byte position of `b` from the start

If `offsetof(Example, b)` is 4, then 3 bytes of padding were inserted after `a`.

---

## 4. The Two Types of Padding

### Type 1 — Internal Padding

Inserted **between members** so that each member sits at its required alignment.

```c
typedef struct {
    char    c1;    // offset 0  (1 byte)
    // [3 bytes padding] ← compiler inserts this
    int32_t i;     // offset 4  (4 bytes, needs 4-byte alignment)
    char    c2;    // offset 8  (1 byte)
    // [7 bytes padding] ← compiler inserts this
    double  d;     // offset 16 (8 bytes, needs 8-byte alignment)
} naive_bad_t;    // total: 24 bytes
```

### Type 2 — Trailing Padding

Inserted **after the last member** so that `sizeof(T)` is a multiple of
`alignof(T)`. This is essential for **arrays** — if trailing padding were
absent, the second element of `T arr[2]` would be misaligned.

```c
typedef struct {
    double  d;    // offset 0  (8 bytes)
    int32_t i;    // offset 8  (4 bytes)
    char    c1;   // offset 12 (1 byte)
    char    c2;   // offset 13 (1 byte)
    // [2 bytes trailing padding] ← makes sizeof = 16 (multiple of 8)
} naive_good_t;  // total: 16 bytes
```

Why 16 and not 14? Because `alignof(naive_good_t) = alignof(double) = 8`.
So `sizeof` must be a multiple of 8. The nearest multiple of 8 above 14 is 16.

**Proof that trailing padding is needed for arrays:**

```c
naive_good_t arr[2];
// arr[0] starts at some address X (must be multiple of 8)
// arr[1] must start at X + sizeof(naive_good_t) = X + 16
// X + 16 is still a multiple of 8 ✓
// Without trailing padding: X + 14 → NOT a multiple of 8 ✗ → arr[1].d is misaligned
```

---

## 5. Alignment Rules — System V AMD64 ABI

The **ABI (Application Binary Interface)** defines the exact alignment each
type must have. On Linux x86-64, this is the System V AMD64 ABI.

| Type | sizeof (bytes) | alignof (bytes) | Notes |
|------|---------------|-----------------|-------|
| `char` / `uint8_t` | 1 | 1 | Can live anywhere |
| `short` / `uint16_t` | 2 | 2 | Must be at even address |
| `int` / `uint32_t` | 4 | 4 | Must be at multiple of 4 |
| `long` / `uint64_t` | 8 | 8 | Must be at multiple of 8 |
| `long long` | 8 | 8 | Same as long on 64-bit |
| `float` | 4 | 4 | Same alignment as int |
| `double` | 8 | 8 | Same alignment as long |
| `long double` | 16 | 16 | 80-bit extended, padded to 16 |
| `void *` (pointer) | 8 | 8 | All pointers are 8 bytes on 64-bit |
| `bool` / `_Bool` | 1 | 1 | Same as char |

**For structs and unions:**
```
alignof(struct T) = max(alignof(member) for all members in T)
sizeof(struct T)  = must be a multiple of alignof(struct T)
```

> **ARM Cortex-M note:** On 32-bit ARM, `long` and pointers are 4 bytes, not 8.
> `long double` is also 8 bytes. Always use `<stdint.h>` fixed-width types
> (`uint32_t`, `uint64_t`) in embedded code to avoid platform surprises.

---

## 6. Optimising Member Ordering

### The Golden Rule

> **Order struct members from largest alignment to smallest.**

This eliminates most internal padding because larger-aligned members placed first
are already at aligned addresses, and smaller members that follow them need less
(or no) alignment correction.

```c
/* BAD — members in random order */
typedef struct {
    char    name;      // 1 byte  → 3 bytes padding after
    int     count;     // 4 bytes → 7 bytes padding after
    char    flag;      // 1 byte
    double  value;     // 8 bytes
} bad_t;               // sizeof = 24 bytes, 10 wasted

/* GOOD — largest alignment first */
typedef struct {
    double  value;     // 8 bytes (alignof=8) — goes first
    int     count;     // 4 bytes (alignof=4) — no padding needed
    char    name;      // 1 byte  (alignof=1) — no padding needed
    char    flag;      // 1 byte  (alignof=1) — no padding needed
                       // 2 bytes trailing padding (to reach multiple of 8)
} good_t;              // sizeof = 16 bytes, only 2 wasted
```

### Real-World Impact

| Scenario | Savings |
|----------|---------|
| Array of 1 million `bad_t` structs | 8 MB |
| Embedded RTOS with 500 task control blocks | 4 KB of RAM |
| Network packet buffers (10,000 entries) | 80 KB |

On a microcontroller with 64 KB of RAM, saving 4 KB is the difference between
fitting your application or not.

### When NOT to Reorder

- **Hardware register maps** — registers have a fixed, hardware-defined layout. Never reorder.
- **Wire protocol frames** — network packets have a defined byte order. Never reorder.
- **Shared memory between processes or cores** — both sides must agree on layout.
- **EEPROM / Flash data structures** — changing layout breaks backwards compatibility.

In these cases, use `__attribute__((packed))` and `ASSERT_SIZE` to lock the layout down.

---

## 7. Bit-fields

Bit-fields let you pack multiple small flags or values into a single storage unit,
saving memory when you have many boolean flags or small integer ranges.

```c
typedef struct {
    uint8_t  ready   : 1;   // occupies bit 0
    uint8_t  error   : 1;   // occupies bit 1
    uint8_t  mode    : 3;   // occupies bits 2-4 (values 0-7)
    uint8_t  channel : 3;   // occupies bits 5-7 (values 0-7)
} hw_flags_t;               // sizeof = 1 byte total!
```

Without bit-fields, 8 separate `uint8_t` members for these flags would cost 8
bytes. With bit-fields, all 8 bits fit in 1 byte — **8x memory reduction**.

### Bit-field Rules and Limitations

| Rule | Detail |
|------|--------|
| Storage unit | Bit-fields are packed into the declared underlying type (`uint8_t`, `uint32_t`, etc.) |
| No crossing | A bit-field will not cross a storage unit boundary — a new unit starts if needed |
| `offsetof` forbidden | You cannot use `offsetof` on a bit-field member (C standard §7.19) — it is a compile error |
| Bit order | The bit ordering within a storage unit is **implementation-defined** (compiler/CPU dependent) |
| Signed bit-fields | Avoid `int` bit-fields — signed behaviour for negative values is implementation-defined. Use `uint8_t` or `uint32_t` |
| Portability | Bit-field layout differs between compilers and architectures — not safe for wire protocols |

### Reading a Bit-field as a Raw Byte

```c
hw_flags_t f = {0};
f.ready   = 1;    // bit 0 set
f.mode    = 5;    // bits 2-4: binary 101
f.channel = 3;    // bits 5-7: binary 011

// Raw byte: 0111 0101 = 0x75
uint8_t raw = *((uint8_t *)&f);
// ready=1, error=0, mode=5, channel=3
```

### When to Use Bit-fields

**Good uses:**
- Packing many boolean flags into one register
- Matching hardware peripheral register definitions (with care about bit order)
- Memory-constrained RTOS task status flags

**Avoid for:**
- Network protocol fields (bit order not guaranteed across compilers)
- Inter-processor shared memory
- Any context where portability across compilers matters

---

## 8. `__attribute__((packed))` When and Why

The `packed` attribute tells GCC/Clang to suppress all padding. Every member
is placed at the very next byte, regardless of alignment.

```c
typedef struct __attribute__((packed)) {
    char    c1;    // offset 0
    int32_t i;     // offset 1  ← NOT aligned to 4! Unaligned access!
    char    c2;    // offset 5
    double  d;     // offset 6  ← NOT aligned to 8! Unaligned access!
} packed_demo_t;   // sizeof = 14 bytes (zero padding)
```

The struct is 14 bytes instead of 24 — but every non-char member is unaligned.

### The Danger: Unaligned Accesses

| Platform | What happens |
|----------|-------------|
| x86-64 | Works, but slower (hardware handles it with extra bus cycles) |
| ARM Cortex-M3/M4/M7 | Works, but slow. Hardware handles it |
| ARM Cortex-M0/M0+ | **CRASH** — UsageFault exception. No hardware support |
| MIPS (many MCUs) | **CRASH** — Address Error exception |
| RISC-V (base ISA, no C ext) | **CRASH** — Load Address Misaligned exception |
| Hardware DMA | **SILENT CORRUPTION** — DMA engines require aligned addresses |

### Safe Uses of `packed`

```c
/* Hardware register map — must match silicon exactly */
typedef struct __attribute__((packed)) {
    uint8_t  status;
    uint16_t data;
    uint8_t  control;
} uart_regs_t;
ASSERT_SIZE(uart_regs_t, 4);   // will fail to compile if layout drifts

/* Wire protocol frame — must match spec byte-for-byte */
typedef struct __attribute__((packed)) {
    uint8_t  start_byte;
    uint16_t length;
    uint8_t  type;
    uint8_t  payload[64];
    uint16_t crc;
} can_frame_t;
```

### Safe Reading Pattern for Packed Fields

When you need to read a multi-byte field from a packed struct on a
strict-alignment architecture, always copy it to a local variable first:

```c
packed_demo_t *p = get_packet();

/* DANGEROUS on strict-align architectures */
int32_t val = p->i;

/* SAFE — memcpy handles unaligned source */
int32_t val;
memcpy(&val, &p->i, sizeof(val));
```

---

## 9. Cache-Line Awareness

Modern CPUs do not fetch individual bytes from RAM — they fetch entire
**cache lines** (64 bytes on x86-64, typically 32 or 64 bytes on ARM Cortex-A).

If two frequently-used fields in a struct happen to fall in the same cache
line, accessing one effectively pre-loads the other for free.

If hot fields are scattered across multiple cache lines, every access may
cause a cache miss — fetching 64 bytes just to use 4 of them, evicting
other useful data in the process.

### Cache-Line Optimised Struct Design

```c
#define CACHE_LINE_SIZE  64U

typedef struct {
    /*
     * HOT PATH — fields touched on every interrupt / every loop iteration.
     * Group them in the first 64 bytes (one cache line).
     * The explicit pad fills any remaining space to the cache line boundary.
     */
    uint32_t         counter;          // offset  0  — incremented in ISR
    uint32_t         flags;            // offset  4  — checked in ISR
    volatile uint32_t status;          // offset  8  — hardware register shadow
    uint8_t          _hot_pad[52];     // offset 12  — explicit fill to 64 bytes

    /*
     * COLD PATH — fields written once at init, or on error only.
     * These live in cache line 1+, so a hot-path access never loads them.
     */
    char             name[32];         // offset 64
    uint32_t         version;          // offset 96
    uint32_t         crc;              // offset 100
} __attribute__((aligned(CACHE_LINE_SIZE))) device_ctx_t;
```

The `aligned(64)` attribute ensures the struct always starts at a 64-byte
boundary, so "cache line 0" of the struct is always a real CPU cache line.

### False Sharing in Multi-Core Systems

When two CPU cores write to different fields that happen to share a cache line,
the CPU cache coherence protocol forces the cache line to be bounced between
cores on every write — even though they are touching different data. This is
called **false sharing** and can reduce multi-core performance dramatically.

Fix: use `aligned(CACHE_LINE_SIZE)` to put each core's data in a separate
cache line, or separate hot/cold data using the grouping technique above.

---

## 10. Flexible Array Members (FAM)

A **flexible array member** is a zero-length array at the end of a struct
(C99 and later). It lets you allocate a variable-length payload contiguous
with the struct header — no separate heap allocation, no pointer indirection.

```c
typedef struct {
    uint32_t length;     // number of payload bytes
    uint16_t checksum;
    uint8_t  type;
    uint8_t  payload[];  // FAM — zero size in sizeof(), but real bytes follow
} flex_packet_t;
```

### Key Rules

- The FAM must be the **last member** of the struct.
- `sizeof(flex_packet_t)` returns the size of the **header only** — it does
  NOT include any space for `payload`. On this platform: 8 bytes.
- You **cannot** declare a plain `flex_packet_t` variable — only pointers or
  `malloc`-allocated instances.

### Allocating a FAM Struct

```c
uint32_t payload_bytes = 32;

/* Allocate header + payload in one contiguous block */
flex_packet_t *pkt = malloc(sizeof(flex_packet_t) + payload_bytes);

pkt->length   = payload_bytes;
pkt->checksum = compute_crc(data, payload_bytes);
pkt->type     = 0x01;

memcpy(pkt->payload, data, payload_bytes);  // safe — payload[] follows header

free(pkt);
```

### FAM vs Pointer Member

```c
/* Approach A: FAM */
typedef struct { uint32_t len; uint8_t data[]; } fam_t;
// malloc(sizeof(fam_t) + N) → 1 allocation, 1 free, cache-friendly

/* Approach B: Pointer */
typedef struct { uint32_t len; uint8_t *data; } ptr_t;
// malloc(sizeof(ptr_t)) + malloc(N) → 2 allocations, 2 frees,
// extra pointer dereference, data may be far away in heap
```

FAM is preferred when:
- The payload size is determined at allocation time and never resized.
- Cache locality matters (header + data in one contiguous block).
- You want to minimise heap fragmentation.

---

## 11. ELF Memory Sections Explained

When you compile a C program, the compiler and linker organise your variables
and code into named **sections** in the ELF (Executable and Linkable Format)
binary file. Each section has different properties.

### The Four Core Sections

```
 ---------------------------------------------------------------------
│                          ELF FILE ON DISK                           │
│                                                                     │
│   ----------   Read-only, executable.                               │
│  │  .text   │  Contains all compiled machine code (function bodies).│
│  │          │  String literals may also land here on some targets.  │
│   ----------                                                        │
│                                                                     │
│   ----------   Read-only, NOT executable.                           │
│  │ .rodata  │  Contains: const global variables, string literals,   │
│  │          │  lookup tables, vtables.                              │
│   ----------                                                        │
│                                                                     │
│   ----------   Read-write, initialised.                             │
│  │  .data   │  Contains: global and static variables with a non-zero│
│  │          │  initial value. The initial values are STORED IN THE  │
│   ----------   FILE — they consume flash on embedded targets.       │
│                                                                     │
│   ----------   Read-write, zero-initialised.                        │
│  │  .bss    │  Contains: global and static variables with no init   │
│  │          │  or explicit = 0. NO SPACE IN FILE — the OS/startup   │
│  │(NO DATA) │  code zero-fills this region before main() runs.      │
│   ----------                                                        │
│                                                                     │
 ---------------------------------------------------------------------
```

### Which Variable Goes Where?

```c
/* .rodata — const qualifier → read-only section */
const uint32_t MAGIC = 0xDEADBEEFU;

/* .rodata — string literals are read-only constants */
const char *msg = "hello";          // "hello" → .rodata, msg ptr → .data

/* .data — non-const, non-zero initialiser */
uint32_t counter = 42;

/* .data — static with non-zero initialiser */
static uint32_t retry_limit = 3;

/* .bss — no initialiser (implicitly zero) */
uint32_t error_count;

/* .bss — explicit zero initialiser (compiler treats same as no init) */
uint8_t rx_buffer[256] = {0};

/* .bss — static with no initialiser */
static uint32_t frame_count;

/* Stack — automatic (local) variable, not in any ELF section */
void foo(void) {
    uint32_t local = 5;   // lives on the call stack
}
```

### Why `.bss` is Critical for Embedded Systems

On a microcontroller, program storage is **flash** (e.g. 512 KB) and runtime
storage is **RAM** (e.g. 64 KB). The ELF file lives in flash.

- `.data` section content must be stored in flash (the initial values), then
  **copied to RAM** by the startup code (crt0) before `main()` runs.
- `.bss` section content is NOT stored in flash. The startup code simply
  **zeroes** the RAM region. No flash consumed.

```
Flash (program storage):
 ------------------------------------------------------------
│     .text        │    .rodata       │  .data initial vals  │
│  (machine code)  │  (const data)    │  (copied to RAM)     │
└──────────────────┴──────────────────┴──────────────────────┘

RAM (runtime storage):
 --------------------------------------------------------------
│ .text*  │     .data        │      .bss        │ heap │ stack │
│(XIP MCU)│(copied from flash│(zero-filled by   │  ↑  │   ↓    │
│         │ at startup)      │ startup code)    │     │        │
 --------------------------------------------------------------
* Some MCUs execute .text directly from flash (XIP = Execute In Place)
```

**Practical rule:** A 1 KB `uint8_t rx_buffer[1024]` in `.bss` costs 0 bytes
of flash. The same array in `.data` with a non-zero initialiser costs 1 KB of
flash. On a 64 KB flash MCU, this matters enormously.

### Verifying Section Placement

```bash
# See the section each symbol lives in (T=text, R=rodata, D=data, B=bss)
nm --numeric-sort ./build/struct.exe | grep -E "g_magic|g_zero|g_init|g_rx"

# Example output:
# 0000000000404040 B g_zero_counter    ← B = .bss
# 0000000000404060 B g_rx_buffer       ← B = .bss
# 0000000000403010 D g_initialized_counter  ← D = .data
# 0000000000402000 R g_magic_number    ← R = .rodata
```

---

## 12. The Stack and the Heap

These are the two **runtime** memory regions that do not exist as ELF sections.

### The Stack

The stack holds **automatic (local) variables** — variables declared inside
functions without `static`. It works like a physical stack of plates: you push
(allocate) when entering a function, pop (free) when returning.

```
High addresses
 -------------------------    - stack base (set by OS at program start)
│   main() frame          │
│   local variables of    │   - Stack Pointer (SP) after main() enters
│   main                  │
 -------------------------
│   foo() frame           │   - SP after foo() is called
│   local variables of    │
│   foo                   │
 -------------------------
│   bar() frame           │   - SP after bar() is called from foo()
│   local variables of    │
│   bar                   │
└─────────────────────────┘   - stack grows downward (lower addresses)
Low addresses
```

**Stack characteristics:**
- Allocation and deallocation are instant (just move the stack pointer)
- Variables disappear when the function returns — never return a pointer to a local!
- Fixed maximum size (typically 1-8 MB on Linux, 512 bytes-4 KB on MCUs)
- Stack overflow causes a crash (SegFault on Linux, HardFault on ARM)

### The Heap

The heap holds **dynamically allocated memory** (`malloc`, `calloc`, `new`).
It is explicitly managed by the programmer.

```c
/* Heap allocation */
uint8_t *buf = malloc(1024);   // request 1024 bytes from heap
if (buf == NULL) { /* handle failure */ }

/* use buf ... */

free(buf);                      // return memory to heap
buf = NULL;                     // prevent dangling pointer use
```

**Heap characteristics:**
- Allocation can be slow and may fragment over time
- Memory persists until explicitly freed — forgetting `free()` is a memory leak
- On embedded systems without an OS, heap is often avoided entirely
  (static allocation only) to guarantee deterministic behaviour

---

## 13. Common Pitfalls

### Pitfall 1 — `memcmp` on Padded Structs

Padding bytes contain **unspecified garbage values** (whatever happened to be on
the stack or in that heap block previously). `memcmp` compares every byte
including padding — even if all named fields are equal, `memcmp` may say
the structs are different.

```c
naive_bad_t a, b;

/* WRONG — padding bytes are garbage, memcmp unreliable */
a.c1 = 'X'; a.i = 1; a.c2 = 'Y'; a.d = 3.14;
b.c1 = 'X'; b.i = 1; b.c2 = 'Y'; b.d = 3.14;
if (memcmp(&a, &b, sizeof(a)) == 0) { /* might NEVER be true! */ }

/* CORRECT — zero the entire struct before populating */
memset(&a, 0, sizeof(a));
memset(&b, 0, sizeof(b));
a.c1 = 'X'; a.i = 1; a.c2 = 'Y'; a.d = 3.14;
b.c1 = 'X'; b.i = 1; b.c2 = 'Y'; b.d = 3.14;
if (memcmp(&a, &b, sizeof(a)) == 0) { /* reliable */ }
```

An alternative is to compare field-by-field with a custom equality function.

### Pitfall 2 — `sizeof` on a Flexible Array Member Struct

```c
flex_packet_t *pkt = malloc(sizeof(flex_packet_t) + 32);

/* WRONG — only copies the 8-byte header, loses the payload! */
flex_packet_t copy;
memcpy(&copy, pkt, sizeof(flex_packet_t));

/* CORRECT — include payload size */
flex_packet_t *copy = malloc(sizeof(flex_packet_t) + pkt->length);
memcpy(copy, pkt, sizeof(flex_packet_t) + pkt->length);
```

### Pitfall 3 — Returning a Pointer to a Local Variable

```c
/* DANGLING POINTER — stack frame destroyed on return */
int *get_value(void) {
    int local = 42;
    return &local;   // local is gone after return → undefined behaviour
}

/* CORRECT — use static, heap, or pass a pointer in */
int *get_value(void) {
    static int value = 42;  // lives in .data, persists
    return &value;
}
```

### Pitfall 4 — Bit-field Order Not Portable

```c
/* This packs bits differently on different compilers/architectures */
typedef struct {
    uint8_t flag_a : 1;
    uint8_t flag_b : 1;
} flags_t;

/* On GCC/x86: flag_a is bit 0, flag_b is bit 1
   On some embedded compilers: flag_a may be bit 7, flag_b bit 6
   → Never use bit-fields in a wire protocol struct */

/* PORTABLE: use explicit masks */
#define FLAG_A_MASK  0x01U
#define FLAG_B_MASK  0x02U
uint8_t flags = 0;
flags |= FLAG_A_MASK;   // always bit 0, on every compiler
```

### Pitfall 5 — `sizeof` Without Parentheses in Macros

```c
/* WRONG — sizeof T without parens in a macro expansion can
   cause operator precedence bugs */
#define ARRAY_BYTES(T, N)  N * sizeof T    // breaks: sizeof T+1 == sizeof(T+1)

/* CORRECT — always parenthesise both the operator and the operand */
#define ARRAY_BYTES(T, N)  ((N) * sizeof(T))
```

### Pitfall 6 — Casting Packed Pointer to Normal Pointer

```c
/* UNDEFINED BEHAVIOUR — violates alignment rules */
packed_demo_t *packed = get_packed_data();
naive_bad_t   *normal = (naive_bad_t *)packed;  // packed may be misaligned!
int32_t val = normal->i;                         // crash on strict-align arch

/* CORRECT — copy then access */
naive_bad_t normal;
memcpy(&normal, packed, sizeof(normal));
int32_t val = normal.i;
```

---

## 14. Compile-Time Safety with `_Static_assert`

`_Static_assert` (C11) fires a **compile-time error** — not a runtime check.
Use it to catch layout drift the moment a developer changes a struct.

```c
/* Will refuse to compile if the struct is ever the wrong size.
   Essential for hardware register maps and protocol definitions. */
_Static_assert(sizeof(uart_regs_t)  ==  4, "uart_regs_t must be exactly 4 bytes");
_Static_assert(sizeof(can_frame_t)  == 72, "can_frame_t ABI frozen at 72 bytes");
_Static_assert(alignof(device_ctx_t) == 64, "device_ctx_t must be cache-line aligned");
```

The project's `ASSERT_SIZE` and `ASSERT_ALIGN` macros in `alignment.h` wrap
this with a cleaner interface:

```c
ASSERT_SIZE(hw_flags_t, 1);      // expands to _Static_assert(sizeof(...)==1, ...)
ASSERT_ALIGN(double, 8);         // expands to _Static_assert(alignof(...)==8, ...)
```

**When to use:** On every struct that interfaces with hardware, network
protocols, files, or inter-processor shared memory. ABI stability is a
contract — `_Static_assert` enforces it automatically on every build.

---

## 15. Useful Toolchain Commands

```bash
# Inspect the ELF binary

# Summary of section sizes (text, data, bss, dec, hex)
size ./build/struct.exe

# Detailed section map with virtual addresses
objdump -h ./build/struct.exe

# All symbols with their addresses and section (B=bss, D=data, R=rodata, T=text)
nm --numeric-sort ./build/struct.exe

# Filter to specific symbols
nm --numeric-sort ./build/struct.exe | grep -E "g_magic|g_zero|g_init"


# Inspect padding in a struct

# Build and watch for -Wpadded warnings (shows every inserted padding byte)
gcc -Wpadded -I./include -c ./src/alignment.c

# Use pahole (Package: dwarves) — prints struct layout from debug info
pahole ./build/struct.exe | grep -A 20 "naive_bad_t"

# Example pahole output:
# struct naive_bad_t {
#     char    c1;          /*     0     1 */
#     /* XXX 3 bytes hole, try to pack */
#     int     i;           /*     4     4 */
#     char    c2;          /*     8     1 */
#     /* XXX 7 bytes hole, try to pack */
#     double  d;           /*    16     8 */
#     /* size: 24, cachelines: 1, members: 4 */
#     /* sum members: 14, holes: 2, sum holes: 10 */
# };

```

---

## 16. Quick Reference Cheat Sheet

### Struct Design Rules

| Rule | When it applies |
|------|----------------|
| Order members largest -> smallest alignment | All general-purpose structs |
| Use `_Static_assert` on size and alignment | Register maps, wire protocols, EEPROM |
| `memset` to 0 before `memcmp` | Any padded struct comparison |
| Never `memcmp` without prior zeroing | Structs with padding |
| Use `memcpy` to access packed struct fields | Packed structs on strict-align targets |
| Keep hot fields in first cache line | High-frequency ISR / loop data |
| Use `.bss` for large zero-init buffers | Embedded systems with flash constraints |
| FAM over pointer member for variable-length data | Protocol buffers, message queues |

### Padding Calculation Formula

```
internal_padding_before_member =
    (alignof(member) - (current_offset % alignof(member))) % alignof(member)

trailing_padding =
    (alignof(struct) - (raw_size % alignof(struct))) % alignof(struct)

sizeof(struct) = sum(sizeof(members)) + sum(internal_padding) + trailing_padding
```

### Section Placement Decision Tree

```
Is the variable const?
  YES -> .rodata (read-only, no writes allowed at runtime)
  NO  -> Is it a global or static?
          YES -> Does it have a non-zero initialiser?
                  YES -> .data  (costs flash on embedded)
                  NO  -> .bss   (zero-filled for free, no flash cost)
          NO  -> Stack (automatic, freed when function returns)
                 or Heap (if malloc'd, freed when free() is called)
```

### Alignment at a Glance (x86-64 Linux)

```
Type         Bytes   Align   Notes
──────────── ─────── ─────── ───────────────────────────────────
char          1       1      Can live at any address
short         2       2      Must be at even address
int           4       4      Must be at multiple of 4
long          8       8      Must be at multiple of 8
long long     8       8      Same as long on 64-bit
float         4       4      Same alignment as int
double        8       8      Same alignment as long
long double  16      16      80-bit x87, padded to 128-bit
void *        8       8      All pointers are 8 bytes on 64-bit
bool          1       1      Same as char
```

> **On 32-bit ARM (Cortex-M):** `long` = 4 bytes, pointers = 4 bytes,
> `long double` = 8 bytes. Always use `<stdint.h>` types for portability.
