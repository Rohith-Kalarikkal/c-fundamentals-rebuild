# Embedded C: `volatile` & `const`

## Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Topics Covered](#topics-covered)
- [Key Concepts](#key-concepts)
- [What Bug Happens if volatile Removed](#what-bug-happens-if-volatile-removed)
- [Demonstrations](#demonstrations)
- [Build Instructions](#build-instructions)
- [Running the Program](#running-the-program)
- [Expected Output](#expected-output)
- [Author](#author)

---

## Overview

On a microcontroller (ARM Cortex-M, AVR, MSP430 …) the compiler is unaware that hardware peripherals change memory independently of C code. Without the right qualifiers, an optimising compiler (`-O2`, `-O3`) will generate incorrect machine code that:

- Caches register values in CPU registers and never re-reads them from the bus
- Eliminates writes it considers "dead" because no C code reads the result
- Merges or reorders register writes, breaking peripheral timing

This project simulates UART, GPIO, Flash, and interrupt scenarios on a host PC to illustrate these problems and their solutions — without needing physical hardware.

---

## Project Structure

```
volatile_const_isr/
│
├── Makefile                              <- Build system (gcc, -O2, MISRA-inspired flags)
├── main.c                                <- Entry point, runs all three programs
├── README.md
├── .gitignore
│
├── include/                              <- Shared headers (used by all modules)
│   ├── platform_types.h                  <- REG32 macro, BIT/SET/CLR helpers, status_t
│   ├── hw_registers.h                    <- UART + GPIO register map structs
│   ├── flash_const_data.h                <- const / volatile const patterns
│   └── isr_flags.h                       <- Volatile ISR flag declarations
│
├── 01_volatile_hardware_registers/       
│   ├── hw_registers.c                    <- Peripheral simulation & register dump
│   └── volatile_registers.c              <- Polling TX/RX, RXNEIE enable
│
├── 02_const_flash_memory/                
│   ├── flash_const_data.c                <- Sine LUT, FW version, error table, Device-ID
│   └── const_flash.c                
│
└── 03_isr_flag_simulation/               
    ├── isr_flags.c                       <- ISR bodies + main-loop handlers
    └── run_isr_flags.c                   <- Simulated super-loop with deferred processing
```

---

## Topics Covered

| # | Topic | File(s) |
|---|-------|---------|
| 1 | Why `volatile` is mandatory for hardware registers | `hw_registers.h`, `hw_registers.c` |
| 2 | Simulating memory-mapped peripheral register maps | `hw_registers.c`, `volatile_registers.c` |
| 3 | Why `const` saves SRAM by placing data in Flash | `flash_const_data.h`, `flash_const_data.c` |
| 4 | The `volatile const` combination for read-only HW registers | `flash_const_data.h`, `flash_const_data.c` |
| 5 | ISR ↔ main-loop communication via volatile flags | `isr_flags.h`, `isr_flags.c` |
| 6 | `MEMORY_BARRIER` to prevent compiler reordering | `isr_flags.c` |
| 7 | Race condition: why flag must be cleared AFTER data read | `isr_flags.c` - `handle_uart_rx()` |

---

## Key Concepts

### `volatile` - Tell the Compiler: "Don't Trust Your Cache"

```c
// WRONG - compiler may read SR once and loop forever at -O2
uint32_t *SR = (uint32_t *)0x40011000;
while (!(*SR & TXE_BIT)) { }   // SR cached in CPU register -> infinite loop

// CORRECT - volatile forces a fresh bus read every iteration
volatile uint32_t *SR = (volatile uint32_t *)0x40011000;
while (!(*SR & TXE_BIT)) { }   // re-reads from peripheral every cycle
```

Hardware registers change **outside the compiler's view** - driven by DMA engines, peripheral state machines, and interrupts. `volatile` is the contract between you and the compiler: *"this memory location can change at any time; always go to the bus."*

---

### `const` — Tell the Linker: "Put This in Flash, Not SRAM"

```c
// WITHOUT const -> 512 bytes consumed from scarce SRAM on every boot
uint16_t sine_lut[256] = { 0, 25, 50, ... };

// WITH const -> linker places table in .rodata (Flash); zero SRAM cost
const uint16_t sine_lut[256] = { 0, 25, 50, ... };
```

On a typical Cortex-M4 MCU: Flash = 512 KB, SRAM = 128 KB. Every lookup table, error string, and configuration struct that is `const` frees precious SRAM for stack and heap. The compiler also generates an error if application code accidentally writes to a `const` variable - catching bugs at compile time rather than as a runtime bus fault.

---

### `volatile const` — Read-Only Hardware Registers

Some registers are written by hardware and must never be written by software (chip ID, silicon revision, eFuse values):

```c
typedef struct {
    volatile const uint32_t CHIP_ID;     // volatile -> re-read every access
    volatile const uint32_t REVISION;    // const    -> software CANNOT write
    volatile const uint32_t UNIQUE_ID0;
} DeviceID_RegMap_t;
```

- `volatile` - hardware (or factory programming) can change this. Compiler must re-read
- `const` - attempting a software write is a compile-time error (and a bus fault on real HW)

---

### ISR Flag Pattern

```
 *    --------------   sets volatile flag     --------------------------
 *   │  Hardware    │ ---------------------> │  ISR (short, fast)       │
 *   │  Peripheral  │                        │  g_uart_rx_flag = true;  │
 *    --------------                         │  g_uart_rx_byte = DR;    │
 *                                            --------------------------
 *                                                       │ flag visible to
 *                                                       │ main after ISR ret
 *                                             --------------------------
 *                                            │  Main loop               │
 *                                            │  if (g_uart_rx_flag) {   │
 *                                            │    handle_uart_rx();     │
 *                                            │  }                       │
 *                                             --------------------------
```

The ISR must be **short and non-blocking** (no `printf`, no `malloc`, no loops). It only captures data and sets a flag. The main loop does the heavy lifting.

---

## What Bug Happens if volatile Removed

This is the most important section — these are **real production bugs** that have caused recalls and safety incidents.

### Bug 1 — Polling Loop Hangs Forever (Load Hoisting)

```c
// Source code (looks correct)
while (!(UART1->SR & TXE_BIT)) { }  // wait for transmit buffer empty

// What gcc -O2 ACTUALLY generates without volatile:
r0 = load(UART1->SR)         // read SR once, before the loop
if (r0 & TXE_BIT == 0):
    loop_forever:             // SR never re-read -> infinite hang
        jump loop_forever
```

**Result:** The MCU hangs. The UART never transmits. The watchdog resets the system.

---

### Bug 2 - Write to Data Register Eliminated (Dead-Store)

```c
// Source code
UART1->DR = 'A';   // trigger hardware to send byte 'A'

// What gcc -O2 generates without volatile:
// <nothing> - the compiler sees DR is never READ back,
//             concludes the write is "dead", and removes it entirely.
```

**Result:** Byte is never sent. Silent data loss - no error, no crash, just missing data on the wire.

---

### Bug 3 - ISR Flag Never Seen (Stale Cache)

```c
// Source code (main loop)
while (!g_uart_rx_flag) { }  // wait for ISR to set the flag

// What gcc -O2 generates without volatile:
r0 = load(g_uart_rx_flag)    // read flag once
if (r0 == false):
    loop_forever:             // flag never re-read -> ISR fires but main
        jump loop_forever     // never wakes up
```

**Result:** The ISR fires and sets the flag in memory, but main loop is stuck on the cached `false` value. The received byte is never processed.

---

### Bug 4 - Register Write Merging (Store Sinking)

```c
// Source code (initialise UART in two steps)
UART1->CR1 = UART_CR1_UE;                    // Step 1: enable UART
UART1->CR1 |= UART_CR1_TE | UART_CR1_RE;    // Step 2: enable TX + RX

// What gcc -O2 may generate without volatile:
UART1->CR1 = UART_CR1_UE | UART_CR1_TE | UART_CR1_RE;  // merged into one write
// The intermediate state (UE set alone) is lost.
// On some peripherals, this intermediate state is required for correct init.
```

**Result:** Peripheral initialises incorrectly. Intermittent failures that are nearly impossible to reproduce in a debugger (because attaching a debugger slows the CPU and changes timing).

---

## Demonstrations

### `volatile` Hardware Register Simulation
**Files:** `01_volatile_hardware_registers/`

Simulates a full UART peripheral register map (`SR`, `DR`, `BRR`, `CR1`, `CR2`) with a state-machine tick function that mimics real hardware. Demonstrates:
- Blocking TX: poll `TXE` → write `DR` → poll `TC`
- Blocking RX: poll `RXNE` → read `DR`
- Interrupt-driven mode: set `RXNEIE` in `CR1`
- Human-readable register dump after each operation

---

### `const` in Flash Memory
**Files:** `02_const_flash_memory/`

Demonstrates all common use-cases for `const` in embedded firmware:
- **Sine LUT** — 256-sample Q0.12 table saved in `.rodata` (no SRAM cost)
- **Firmware version struct** — `const` struct with `__DATE__` / `__TIME__` build stamps
- **Error string table** — `const` array of code→string pairs (strings in Flash)
- **Device-ID registers** — `volatile const` read-only hardware register map

---

### ISR Flag Simulation
**Files:** `03_isr_flag_simulation/`

Simulates a bare-metal super-loop responding to three interrupt sources:
- UART RX ISR (`g_uart_rx_flag`)
- SysTick / Timer ISR (`g_timer_tick_flag`, `g_tick_ms`)
- GPIO EXTI Button ISR (`g_button_pressed_flag`)

Shows correct flag ordering (write data -> barrier -> set flag), correct clearing (read data -> then clear flag), and priority-ordered handling in the main loop.

---

## Build Instructions

| GCC | 10.x or later |
| GNU Make | 4.x or later |

---

### Using Make

```bash
make          # compile all source files -> produces ./pointers_demo
make run      # compile (if needed) then run immediately
make clean    # remove all
make help     # List all the make instructions
```

---

## Running the Program

```bash
# Full build + run in one step:
make run

# Or run the already-compiled binary directly:
./build/vol_con_isr.exe
```

---

## Expected Output

```
------------------------------------------------------------------
               Embedded C: volatile & const & ISR
------------------------------------------------------------------

--------------------------------------------------------------------------
                   Volatile Hardware Register Simulation
--------------------------------------------------------------------------

--- Initialising peripherals ------------------------------------
[HW_INIT] UART1 registers initialised. SR=0x000000C0 CR1=0x0000200C

---- UART Register Dump: After init ---------------------
  SR  = 0x000000C0  [TXE=1 TC=1 RXNE=0 ORE=0
  DR  = 0x00000000  (last byte = '.')
  BRR = 0x00000683
  CR1 = 0x0000200C  [UE=1 TE=1 RE=1 RXNEIE=0 TXEIE=0]
--------------------------------------------------------------------------

--- Transmitting byte 'A' (0x41) --------------------------------
  [TX] Waiting for TXE (transmit buffer empty)...
  [TX] Wrote 0x41 ('A') to DR.
  [TX] Waiting for TC (transmission complete)...
  [TX] Byte transmitted successfully.

---- UART Register Dump: After TX 'A' ---------------------
  SR  = 0x00000040  [TXE=0 TC=1 RXNE=0 ORE=0
  DR  = 0x00000041  (last byte = 'A')
  BRR = 0x00000683
  CR1 = 0x0000200C  [UE=1 TE=1 RE=1 RXNEIE=0 TXEIE=0]
--------------------------------------------------------------------------

--- Simulating incoming byte 'Z' (0x5A) -------------------------
  [SIM] Remote device sends byte 0x5A ('Z')

---- UART Register Dump: After remote sends 'Z' ---------------------
  SR  = 0x00000060  [TXE=0 TC=1 RXNE=1 ORE=0
  DR  = 0x0000005A  (last byte = 'Z')
  BRR = 0x00000683
  CR1 = 0x0000200C  [UE=1 TE=1 RE=1 RXNEIE=0 TXEIE=0]
--------------------------------------------------------------------------

  [RX] Waiting for RXNE (receive buffer not empty)...
  [RX] Received 0x5A ('Z')
  [APP] Application received: 'Z'

---- UART Register Dump: After RX consumed ---------------------
  SR  = 0x00000040  [TXE=0 TC=1 RXNE=0 ORE=0
  DR  = 0x0000005A  (last byte = 'Z')
  BRR = 0x00000683
  CR1 = 0x0000200C  [UE=1 TE=1 RE=1 RXNEIE=0 TXEIE=0]
--------------------------------------------------------------------------

--- Enabling RXNE interrupt (interrupt-driven mode) -------------
  RXNEIE set in CR1. CR1 = 0x0000202C

--- volatile lesson summary ----------------------------------
  Without volatile:
    - TXE/RXNE poll loops compile to infinite loops (value cached)
    - DR write may be eliminated (compiler sees no use of written val)
    - CR1 multi-bit writes may be merged (intermediate states lost)
  With volatile:
    - Every access is a real load/store instruction to the bus
    - Compiler cannot reorder accesses across volatile barriers
    - Peripheral state machine sees correct timing

------------------------------------------------------------------


--------------------------------------------------------------------------
                          const in Flash Memory
--------------------------------------------------------------------------
[FLASH_INIT] Device-ID registers seeded (BSP init only).

--- Firmware version (const struct in .rodata) ------------------

  Firmware Version : 2.4.1
  Build Date       : Mar  5 2026
  Build Time       : 16:25:01
  (g_fw_version lives at address 00007FF73B7D5D40 - in .rodata / Flash)

--- Sine LUT (const uint16_t[256] in .rodata) -------------------

  Sine LUT (first 8 / last 8 samples):
  Idx   Value (Q0.12)
  --------------------
  [  0]     0
  [  1]    25
  [  2]    50
  [  3]    75
  [  4]   100
  [  5]   125
  [  6]   150
  [  7]   175
  ...
  [248]  3517
  [249]  3513
  [250]  3509
  [251]  3505
  [252]  3501
  [253]  3496
  [254]  3492
  [255]  3487

  LUT address: 00007FF73B7D5D60 (ROM / .rodata - writes would fault on HW)

--- Device-ID registers (volatile const) ------------------------

  Device ID Registers (volatile const - HW read-only):
  CHIP_ID    = 0x04350001
  REVISION   = 0x00000003
  UNIQUE_ID  = DEADBEEF-CAFEBABE-12345678

  NOTE: 'volatile const' means:
    volatile -> compiler must re-read from HW every access
    const    -> software is forbidden to write (bus error on HW)

--- Error string lookup from Flash table ------------------------
  Error 0 -> "No error"
  Error 1 -> "Generic error"
  Error 2 -> "Peripheral busy"
  Error 3 -> "Timeout waiting for hardware ready"
  Error 4 -> "UART overrun ΓÇö byte dropped"
  Error 5 -> "I2C NACK received from slave"
  Error 6 -> "SPI frame error"
  Error 7 -> "Flash write protection violation"
  Error 8 -> "DMA transfer error"
  Error 9 -> "Watchdog reset triggered"

--- const lesson summary ---------------------------------------
  const placed before variable type:
    - Linker emits symbol to .rodata (-> Flash on MCU)
    - Saves scarce SRAM - LUTs, version structs, error tables
    - Compiler error on accidental writes -> safer code
    - Enables string literal sharing / de-duplication

  volatile const combination:
    - volatile -> compiler cannot cache, every read hits the bus
    - const    -> application code cannot write (HW-enforced RO)
    - Ideal for: chip-ID reg, read-only status registers, eFuse

------------------------------------------------------------------


------------------------------------------------------------------------
               ISR Flag Simulation (volatile flag pattern)
------------------------------------------------------------------------

Scenario: main loop idle, then 3 "interrupts" fire

[LOOP iter 1] No flags set - main loop finds nothing to do.
  (No handlers called - correct idle behaviour)

[EVENT] UART hardware places byte 0x48 ('H') in DR - fires ISR
  [ISR/UART_RX] Byte 0x48 captured. Flag set.

[LOOP iter 2] Checking flags...
  [MAIN] handle_uart_rx: Processing byte 0x48 ('H')

[EVENT] Timer ISR fires (1 ms elapsed)
[EVENT] Timer ISR fires (1 ms elapsed)

[LOOP iter 3] Checking flags (timer_tick only)...
  [MAIN] handle_timer_tick: tick=2

[EVENT] Button GPIO EXTI ΓÇö fires ISR
  [ISR/BUTTON] Button press detected. Flag set.
[EVENT] Another UART byte 0x57 ('W') arrives
  [ISR/UART_RX] Byte 0x57 captured. Flag set.

[LOOP iter 4] Multiple flags pending ΓÇö handle in priority order:
  [MAIN] handle_uart_rx: Processing byte 0x57 ('W')
  [MAIN] handle_button_press: Toggling LED state.

--- ISR flag lesson summary -------------------------------
  volatile bool flag pattern guarantees:
    - Main loop always sees ISR-written flag (no stale cache)
    - ISR stays short - no blocking, no heap, no printf
    - MEMORY_BARRIER prevents reordering of flag vs data writes
    - Clear flag AFTER reading data (avoids race condition)
  When to upgrade beyond simple volatile:
    - Multiple writers -> use atomic_flag or critical section
    - Queue of events  -> use ring buffer protected by critical sec
    - RTOS environment -> use semaphore / event group

------------------------------------------------------------------

All complete.
```

> **Note:** The memory addresses printed for `g_fw_version` and `g_sine_lut` will differ on each run - this is normal. On a real MCU these would be fixed addresses assigned by the linker script.

---

## Author

**Rohith Kalarikkal Ramakrishnan**  
Version: 1.0.0 
Language: C11

---
