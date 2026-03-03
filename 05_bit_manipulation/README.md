# Bit Manipulation — Embedded C Reference Program

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

## Overview

Bit manipulation is the backbone of embedded systems programming. Every hardware peripheral (UART, SPI, GPIO, timers) is controlled by reading and writing individual bits inside memory-mapped registers.

This project has:

- All six C bitwise operators with annotated, visual binary output
- The four core operations: **Set**, **Clear**, **Toggle**, **Check**
- Multi-bit field read/write without disturbing neighbouring bits
- A complete **simulated peripheral register workflow** that mirrors real STM32/AVR/NXP driver code

---

## Project Structure

```
bit_manipulation/
├── include/
│   ├── bit_ops.h        # Core macros: SET_BIT, CLEAR_BIT, TOGGLE_BIT,
│   │                       CHECK_BIT, GET_FIELD, SET_FIELD + declarations
│   └── register_sim.h   # Simulated peripheral register map + API
├── src/
│   ├── main.c           # Entry point, runs all four demos
│   ├── bit_ops.c        # print_binary + three operator demos
│   └── register_sim.c   # Peripheral register control implementation
├── Makefile
└── README.md
```

---

## Topics Covered

| # | Topic |
|---|-------|
| 1 | Bitwise AND `&` — masking / isolating bits |
| 2 | Bitwise OR `\|` — setting bits non-destructively |
| 3 | Bitwise XOR `^` — toggling / detecting changes |
| 4 | Bitwise NOT `~` — complement / inverted mask |
| 5 | Left Shift `<<` — multiply by power-of-2 |
| 6 | Right Shift `>>` — divide by power-of-2 |
| 7 | `SET_BIT` macro |
| 8 | `CLEAR_BIT` macro |
| 9 | `TOGGLE_BIT` macro |
| 10 | `CHECK_BIT` macro |
| 11 | `BIT_MASK(n)` — generate n-bit masks |
| 12 | `GET_FIELD` / `SET_FIELD` — multi-bit register fields |
| 13 | Peripheral register simulation (CTRL / STATUS / DATA) |

---

## Key Concepts

### Core Macros

```c
SET_BIT(REG, BIT)               // REG |=  (1UL << BIT)
CLEAR_BIT(REG, BIT)             // REG &= ~(1UL << BIT)
TOGGLE_BIT(REG, BIT)            // REG ^=  (1UL << BIT)
CHECK_BIT(REG, BIT)             // ((REG >> BIT) & 1UL)
BIT_MASK(WIDTH)                 // (1UL << WIDTH) - 1
GET_FIELD(REG, POS, WIDTH)      // extract multi-bit field
SET_FIELD(REG, POS, WIDTH, VAL) // write multi-bit field non-destructively
```

### Why `1UL` Instead of `1`?

Using `1UL` (unsigned long) prevents **undefined behaviour** from signed integer overflow during left shifts — a real pitfall that causes bugs on 8-bit and 16-bit embedded targets.

### Read-Modify-Write

The most critical pattern in register programming: always preserve bits you don't intend to change.

```c
/* WRONG — wipes all other bits in the register */
CTRL_REG = (1 << ENABLE_BIT);

/* CORRECT — only raises ENABLE_BIT, leaves others untouched */
SET_BIT(CTRL_REG, ENABLE_BIT);
```

### Volatile Registers

All simulated registers use `volatile uint32_t`. On real hardware, the `volatile` keyword prevents the compiler from caching register values in CPU registers — ensuring every read/write actually hits the hardware address.

---

## Demonstrations

### Six Bitwise Operators

Uses `A = 0xAA (1010_1010)` and `B = 0xF0 (1111_0000)` to illustrate every operator with full 32-bit binary output.

```
A & B  →  1010_0000  (0xA0)   Shared 1-bits only
A | B  →  1111_1010  (0xFA)   All 1-bits from either
A ^ B  →  0101_1010  (0x5A)   Bits that differ
~A     →  0101_0101  (0x55)   Every bit flipped
A << 2 →  1010_1000  (0xA8)   Shifted left, LSBs zeroed
B >> 4 →  0000_1111  (0x0F)   Shifted right, upper nibble extracted
```

### Set / Clear / Toggle / Check

Walks through a single byte register from `0x00`, applying each macro step by step and printing the binary state after every operation. Also demonstrates `SET_FIELD` writing `0b101` into bits [6:4].

### Shift as Fast Arithmetic

Shows the relationship between shifts and powers-of-two, plus `BIT_MASK(n)` generating masks from 1-bit (`0x01`) to 8-bit (`0xFF`).

### Peripheral Register Simulation

Simulates a full firmware initialisation and transmit sequence:

```
Step 1  - periph_init()          Reset all registers to 0x00
Step 2  - periph_enable()        SET_BIT(CTRL, 0) - peripheral ON
Step 3  - periph_set_mode(TX)    SET_BIT(CTRL, 1) - TX mode
Step 4  - periph_set_irq(true)   SET_BIT(CTRL, 2) - interrupt enabled
Step 5  - Register snapshot      CTRL_REG = 0x07
Step 6  - periph_write_data()    DATA_REG = 0x3A5 (ch=3, payload=0xA5)
Step 7  - HW asserts BUSY        STATUS_REG bit 0 set by "hardware"
Step 8  - HW asserts TX_EMPTY    Transfer complete; firmware clears BUSY
Step 9  - TOGGLE_BIT(LOOPBACK)   Diagnostic mode on -> off
Step 10 - periph_disable()       CLEAR_BIT(CTRL, 0) - peripheral OFF
```

Final register state confirms `DATA_REG = 0x000003A5`:
- Bits [7:0] = `0xA5` -> payload
- Bits [11:8] = `3` -> channel

---

## Build Instructions

### Prerequisites

| GCC | 10.x or later |
| GNU Make | 4.x or later |

---

### Using Make (Recommended)

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
./pointers_demo
```

---

## Expected Output

```
-------------------------------------------------------
    BIT MANIPULATION - Embedded C Reference Program
    Covers: &  |  ^  ~  <<  >>  SET  CLR  TGL  CHK
-------------------------------------------------------

-------------------------------------------------------
              Six Bitwise Operators
-------------------------------------------------------

  Operands:
  A (0xAA)                       : 0b 0000_0000_0000_0000_0000_0000_1010_1010  (0x000000AA  /        170)
  B (0xF0)                       : 0b 0000_0000_0000_0000_0000_0000_1111_0000  (0x000000F0  /        240)

  1. AND  ( A & B )
     Rule: output bit = 1 only when BOTH inputs are 1.
     Use case: masking - isolate specific bits.
  A & B                          : 0b 0000_0000_0000_0000_0000_0000_1010_0000  (0x000000A0  /        160)

  2. OR   ( A | B )
     Rule: output bit = 1 when AT LEAST ONE input is 1.
     Use case: setting bits without disturbing others.
  A | B                          : 0b 0000_0000_0000_0000_0000_0000_1111_1010  (0x000000FA  /        250)

  3. XOR  ( A ^ B )
     Rule: output bit = 1 when inputs DIFFER.
     Use case: toggling bits, detecting changes.
  A ^ B                          : 0b 0000_0000_0000_0000_0000_0000_0101_1010  (0x0000005A  /         90)

  4. NOT  ( ~A )  - bitwise complement
     Rule: every bit is flipped.
     Note: result is 32-bit, mask to 8-bit for clarity.
  ~A (full 32-bit)               : 0b 1111_1111_1111_1111_1111_1111_0101_0101  (0xFFFFFF55  / 4294967125)
  ~A (masked 8-bit)              : 0b 0000_0000_0000_0000_0000_0000_0101_0101  (0x00000055  /         85)

  5. LEFT SHIFT  ( A << 2 )
     Rule: shift bits toward MSB; fill LSBs with 0.
     Effect: multiply by 2^n (if no bits are lost).
  A << 2                         : 0b 0000_0000_0000_0000_0000_0000_1010_1000  (0x000000A8  /        168)

  6. RIGHT SHIFT ( B >> 4 )
     Rule (unsigned): shift toward LSB, fill MSBs with 0.
     Effect: divide by 2^n (integer, floor).
  B >> 4                         : 0b 0000_0000_0000_0000_0000_0000_0000_1111  (0x0000000F  /         15)

------------------------------------------------------
          Set / Clear / Toggle / Check
------------------------------------------------------

  Initial state: reg = 0x00
  reg                            : 0b 0000_0000_0000_0000_0000_0000_0000_0000  (0x00000000  /          0)

  >> SET_BIT(reg, 3)  - raise bit 3
  reg after set                  : 0b 0000_0000_0000_0000_0000_0000_0000_1000  (0x00000008  /          8)

  >> SET_BIT(reg, 7)  - raise bit 7
  reg after set                  : 0b 0000_0000_0000_0000_0000_0000_1000_1000  (0x00000088  /        136)

  >> SET_BIT(reg, 0)  - raise bit 0 (LSB)
  reg after set                  : 0b 0000_0000_0000_0000_0000_0000_1000_1001  (0x00000089  /        137)

  >> CHECK_BIT - inspect individual bits
     CHECK_BIT(reg, 3) = 1  (SET)
     CHECK_BIT(reg, 5) = 0  (CLEAR)

  >> CLEAR_BIT(reg, 3) - lower bit 3
  reg after clear                : 0b 0000_0000_0000_0000_0000_0000_1000_0001  (0x00000081  /        129)

  >> CLEAR_BIT(reg, 7) - lower bit 7
  reg after clear                : 0b 0000_0000_0000_0000_0000_0000_0000_0001  (0x00000001  /          1)

  >> TOGGLE_BIT(reg, 0) - flip bit 0 (currently SET -> should CLEAR)
  reg after toggle               : 0b 0000_0000_0000_0000_0000_0000_0000_0000  (0x00000000  /          0)

  >> TOGGLE_BIT(reg, 4) - flip bit 4 (currently CLEAR -> should SET)
  reg after toggle               : 0b 0000_0000_0000_0000_0000_0000_0001_0000  (0x00000010  /         16)

  >> TOGGLE_BIT(reg, 4) again - flip back to CLEAR
  reg after toggle               : 0b 0000_0000_0000_0000_0000_0000_0000_0000  (0x00000000  /          0)

  >> SET_FIELD(reg, pos=4, width=3, val=0b101)
     Writing value 5 (0b101) into bits [6:4]
  reg after SET_FIELD            : 0b 0000_0000_0000_0000_0000_0000_0101_0000  (0x00000050  /         80)
     GET_FIELD(reg, 4, 3) = 5  (expected 5)

------------------------------------------------------
     Left Shift as Fast Multiply by Power-of-2
------------------------------------------------------

  val = 1; left-shift up to bit 7:
  Shift   Expression    Value
  ------  ----------    -----
  << 0    1 << 0       1
  << 1    1 << 1       2
  << 2    1 << 2       4
  << 3    1 << 3       8
  << 4    1 << 4       16
  << 5    1 << 5       32
  << 6    1 << 6       64
  << 7    1 << 7       128

  Right-shift - fast integer divide by power-of-2:
  n             Shift   n >> shift
  ---           ------  ----------
  256           >> 0    256
  256           >> 1    128
  256           >> 2    64
  256           >> 3    32
  256           >> 4    16
  256           >> 5    8
  256           >> 6    4
  256           >> 7    2
  256           >> 8    1

  BIT_MASK(n) - generates n consecutive 1-bits:
  BIT_MASK(1) = 0b0000_0001  (0x01)
  BIT_MASK(2) = 0b0000_0011  (0x03)
  BIT_MASK(3) = 0b0000_0111  (0x07)
  BIT_MASK(4) = 0b0000_1111  (0x0F)
  BIT_MASK(5) = 0b0001_1111  (0x1F)
  BIT_MASK(6) = 0b0011_1111  (0x3F)
  BIT_MASK(7) = 0b0111_1111  (0x7F)
  BIT_MASK(8) = 0b1111_1111  (0xFF)

--------------------------------------------------------
        Simulated Peripheral Register Control
--------------------------------------------------------

  Step 1: Initialise peripheral
  [periph_init]  Registers reset to 0x00000000

  Step 2: Enable peripheral
  [periph_enable]  CTRL_ENABLE_BIT set

  Step 3: Set TX mode
  [periph_set_mode] Mode -> TX (bit 1 = 1)

  Step 4: Enable interrupt
  [periph_set_irq]  Interrupt ENABLED  (bit 2 = 1)

  Step 5: Register snapshot after configuration

---------------------------------------------
         PERIPHERAL REGISTER DUMP
  ---------------------------------------------
  CTRL_REG   [7:0]             : 0b 0000_0000_0000_0000_0000_0000_0000_0111  (0x00000007)
      Bit 0 ENABLE    = 1
      Bit 1 MODE      = 1  (TX)
      Bit 2 IRQ_EN    = 1
      Bit 3 LOOPBACK  = 0
      Bit 7 RESET     = 0
  ----------------------------------------------------------------
  STATUS_REG [7:0]             : 0b 0000_0000_0000_0000_0000_0000_0000_0000  (0x00000000)
      Bit 0 BUSY      = 0
      Bit 1 TX_EMPTY  = 0
      Bit 2 RX_FULL   = 0
      Bit 3 OVERFLOW  = 0
      Bit 4 PARITY_ER = 0
  ----------------------------------------------------------------
  DATA_REG  [11:0]             : 0b 0000_0000_0000_0000_0000_0000_0000_0000  (0x00000000)
      Bits[7:0]  PAYLOAD = 0x00 (0)
      Bits[11:8] CHANNEL = 0
  ----------------------------------------------------------------

  Step 6: Write data byte 0xA5 on channel 3
  [periph_write_data] ch=3  data=0xA5 -> DATA_REG=0x000003A5

  Step 7: Hardware asserts BUSY flag
  [hw_event] STATUS bit 0 asserted  (STATUS_REG=0x00000001)

  Step 8: Hardware asserts TX_EMPTY (transfer complete)
  [hw_event] STATUS bit 1 asserted  (STATUS_REG=0x00000003)
  [fw poll] BUSY cleared - transfer finished

  Step 9: Toggle LOOPBACK bit for diagnostics
  LOOPBACK = 1  (now active)
  LOOPBACK = 0  (toggled off)

  Step 10: Disable peripheral
  [periph_disable] CTRL_ENABLE_BIT cleared

  Final register snapshot:

---------------------------------------------
         PERIPHERAL REGISTER DUMP
  ---------------------------------------------
  CTRL_REG   [7:0]             : 0b 0000_0000_0000_0000_0000_0000_0000_0110  (0x00000006)
      Bit 0 ENABLE    = 0
      Bit 1 MODE      = 1  (TX)
      Bit 2 IRQ_EN    = 1
      Bit 3 LOOPBACK  = 0
      Bit 7 RESET     = 0
  ----------------------------------------------------------------
  STATUS_REG [7:0]             : 0b 0000_0000_0000_0000_0000_0000_0000_0010  (0x00000002)
      Bit 0 BUSY      = 0
      Bit 1 TX_EMPTY  = 1
      Bit 2 RX_FULL   = 0
      Bit 3 OVERFLOW  = 0
      Bit 4 PARITY_ER = 0
  ----------------------------------------------------------------
  DATA_REG  [11:0]             : 0b 0000_0000_0000_0000_0000_0011_1010_0101  (0x000003A5)
      Bits[7:0]  PAYLOAD = 0xA5 (165)
      Bits[11:8] CHANNEL = 3
  ----------------------------------------------------------------

--------------------------------------------------------
         All demos completed successfully.
--------------------------------------------------------

```

---

## Author

**Rohith Kalarikkal Ramakrishnan**  
Version: 1.0.0 
Language: C11

---