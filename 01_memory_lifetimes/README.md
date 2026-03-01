# Local vs Static Variable Behavior — Embedded C

> A C11 project demonstrating the behavioral difference between **local (auto)** and **static local** variables, with real-world embedded firmware use-cases.

## Table of Contents

- [Overview](#overview)
- [Project Structure](#project-structure)
- [Key Concepts](#key-concepts)
- [Demonstrations](#demonstrations)
- [Build Instructions](#build-instructions)
- [Running the Program](#running-the-program)
- [Expected Output](#expected-output)
- [Coding Standards](#coding-standards)
- [Author](#author)

---

## Overview

This repository is a  demonstration of **Memory Management in C**, specifically focusing on the differences between **Local (automatic) and Static variables**.

It is designed with an embedded systems mindset, showing how these concepts apply to real-world scenarios like state machines (FSM), hardware initialization guards, and signal edge detection.

---

## Project Structure

```
project/
├── main.c                    # Entry point — orchestration only, zero business logic
│
├── include/                  # All public header files
│   ├── app_types.h           # Shared typedefs  (GpioPinState_t, FsmState_t)
│   ├── app_config.h          # All compile-time constants in one place
│   ├── var_demo.h            # API for core local/static concept demo
│   ├── gpio_edge_counter.h   # API for ISR rising-edge counter module
│   ├── fsm.h                 # API for finite state machine module
│   ├── peripheral_init.h     # API for one-time init guard module
│   └── console_utils.h       # API for console display helpers
│
├── src/                      # Business logic implementations
│   ├── var_demo.c            # Core concept: local vs static counters + addresses
│   ├── gpio_edge_counter.c   # ISR edge counter using static file-scope variables
│   ├── fsm.c                 # FSM with static local state register
│   └── peripheral_init.c     # One-time init guard using static local bool
│
├── utils/                    # Presentation / tooling layer
│   └── console_utils.c       # Banner, separator, summary table
│
├── Makefile                  # make / make debug / make clean
└── README.md                 # This file
```

## Key Concepts

### 1. Memory Lifetime & Scope

***Local Variables***: Allocated on the Stack. They are "forgotten" as soon as the function returns.
```c
void demo(void)
{
    uint32_t counter = 0U;   // Created on the stack every call
    counter++;               // Always 0 → 1; previous value is GONE
    printf("%u\n", counter); // Always prints: 1
}
```

***Static Variables***: Allocated in the Data/BSS segment. They persist for the entire life of the program, acting as "memory" for functions.
```c
void demo(void)
{
    static uint32_t counter = 0U;  // In BSS/Data segment — initialized ONCE
    counter++;                     // Accumulates: 1, 2, 3, 4 …
    printf("%u\n", counter);       // Prints: 1, then 2, then 3 …
}
```

### 2. Practical Embedded Use-Cases

***Edge Counting***: Using static to remember the previous state of a GPIO pin to detect a "Rising Edge."

***Finite State Machines (FSM)***: Using static to track system modes (IDLE, RUNNING, FAULT) without using global variables.

***Init Guards***: Using a static latch to ensure hardware initialization code only runs exactly once.
```
```

## Demonstrations

The project contains five sections, each building on the previous:

### Section 1 — Core Behavior
Calls `VarDemo_RunLocal()` and `VarDemo_RunStatic()` six times each.  
The local counter always prints `1`; the static counter counts up to `6`.

### Section 2 — Memory Address Comparison
Calls `VarDemo_RunAddressComparison()` twice.  
Proves that the local variable's stack address can change between calls while the static variable's data-segment address is identical every time.

### Section 3 — ISR Rising-Edge Counter
Injects a simulated GPIO waveform `LOW, LOW, HIGH, LOW, HIGH, HIGH` into `GPIO_EdgeCounter_Sample()`.  
Two static file-scope variables (`s_prev_state`, `s_rising_edge_count`) detect and count rising edges with no global variables.

### Section 4 — Finite State Machine
Runs the FSM for six scheduler ticks.  
A static local `s_state` inside `FSM_Tick()` drives the transitions:

```
IDLE (tick 1) -> RUNNING (ticks 2–3) -> FAULT (ticks 4–6)
```

### Section 5 — One-Time Initialization Guard
Calls `Peripheral_Init()` six times.  
The static bool `s_initialized` ensures the hardware setup sequence executes exactly once regardless of how many times the function is called.

```
Call 1 -> [INIT] runs
Calls 2–6 -> [SKIP] silently
```

---

## Build Instructions

### Prerequisites

| GCC | 10.x or later |
| GNU Make | 4.x or later |

### Release Build

```bash
make
```

Produces: `build/demo`  
Flags: `-std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wcast-align -Wconversion -O2`

### Debug Build (AddressSanitizer + UBSan)

```bash
make debug
```

Produces: `build/demo` with `-g3 -O0 -fsanitize=address,undefined`  
Use this when investigating memory bugs or undefined behavior.

### Clean

```bash
make clean
```

Removes the entire `build/` directory.

---

## Running the Program

```bash
./build/demo
```

---

## Expected Output

```
   -------------------------------------------------------------
  |          LOCAL vs STATIC Variable Behavior — Demo           |
  |         Embedded C  |  C11  |  Professional Reference       |
   -------------------------------------------------------------

=================================================================
  SECTION: 1. Local vs Static - Core Behavior
=================================================================

  Calling VarDemo_RunLocal()  6 times:

    Local  variable  |  counter =  1  |  &counter = 0x7ffd...  <- changes
    Local  variable  |  counter =  1  |  &counter = 0x7ffd...
    ...

  Calling VarDemo_RunStatic() 6 times:

    Static variable  |  counter =  1  |  &counter = 0x5561...  <- fixed
    Static variable  |  counter =  2  |  &counter = 0x5561...
    Static variable  |  counter =  3  |  &counter = 0x5561...
    ...

=================================================================
  SECTION: 3. Embedded Use-Case: ISR Rising-Edge Counter
=================================================================

    No edge              (prev=LOW , curr=LOW )
    No edge              (prev=LOW , curr=LOW )
    Rising edge #1 detected  (prev=LOW, curr=HIGH)
    No edge              (prev=HIGH, curr=LOW )
    Rising edge #2 detected  (prev=LOW, curr=HIGH)
    No edge              (prev=HIGH, curr=HIGH)

=================================================================
  SECTION: 5. Embedded Use-Case: One-Time Initialization Guard
=================================================================

    [INIT] Peripheral clock enabled.
    [INIT] DMA channels configured.
    [INIT] Interrupt priority set — peripheral READY.
    [SKIP] Peripheral already initialized — skipping init.
    [SKIP] Peripheral already initialized — skipping init.
    ...
```

---

## Coding Standards

This project targets compliance with the following standards:

- **Language:** C11 (`-std=c11`)
- **MISRA-C 2012** alignment:
  - Rule 9.1 - all local variables initialized at declaration
  - Rule 16.4 - all `switch` statements have a `default` clause
  - Rule 2.4 - no unused type declarations
- **Naming conventions:**
  - `Module_FunctionName()` for public API functions
  - `s_variable_name` prefix for static (file/function scope) variables
  - `UPPER_CASE` for macros and constants
  - `TypeName_t` suffix for `typedef` types
- **Compiler warnings treated as errors** in CI (add `-Werror` to `CFLAGS` in your pipeline)

---

## Author

**Rohith Kalarikkal Ramakrishnan**  
Version: 1.0.0 
Language: C11

---
