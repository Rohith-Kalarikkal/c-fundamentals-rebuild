# Pointers in C

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

# Overview

> This project is a structured, production-quality C tutorial focused exclusively
 on **pointers** — one of the most fundamental and misunderstood topics in the
 C language. It is written to the standard expected in professional embedded software teams.

## Project Structure

```
c_pointers_tutorial/
├── README.md
├── Makefile
├── main.c                        # Entry point; runs all modules
├── include/
│   ├── pointer_basics.h          # Declarations: basics & arithmetic
│   ├── array_pointer.h           # Declarations: array vs pointer
│   ├── array_ops.h               # Declarations: reverse, print
│   └── swap.h                    # Declarations: swap via pointer
└── src/
    ├── pointer_basics.c          # Pointer basics & arithmetic
    ├── array_pointer.c           # Array vs pointer differences
    ├── array_ops.c               # Reverse & print using pointers
    └── swap.c                    # Swap using pointers
```
---

## Topics Covered

| # |                    Topic                          |          File          |
|---|---------------------------------------------------|------------------------|
| 1 | Pointer declaration, dereferencing, address-of    | `src/pointer_basics.c` |
| 2 | Pointer arithmetic (++, --, +n, difference)       | `src/pointer_basics.c` |
| 3 | Array vs pointer (similarities & key differences) | `src/array_pointer.c`  |
| 4 | Reverse an array in-place using pointers          | `src/array_ops.c`      |
| 5 | Swap two integers using pointers                  | `src/swap.c`           |
| 6 | Print array by incrementing a pointer             | `src/array_ops.c`      |

---

## Key Concepts

### 1. What Is a Pointer?

A pointer is a variable whose **value is a memory address**. Think of RAM as a long street of numbered houses. A normal variable is a house — it stores data. A pointer is a piece of paper with a house number written on it.

```c
int  x   = 42;      // Normal variable — stores a value
int *ptr = &x;      // Pointer        — stores the ADDRESS of x
int  val = *ptr;    // Dereference    — goes to the address, reads the value (42)
```

`int *ptr`    | Pointer declaration   -> Declares `ptr` as a variable that holds an address
`&var`        | Address-of            -> Returns the memory address of `var`
`*ptr`        | Dereference           -> Goes to the address and reads/writes the value there
`NULL`        | Null pointer          -> Points to nothing — always initialise unused pointers

---

### 2. Pointer Arithmetic

When you add or subtract from a pointer, it moves by multiples of `sizeof` the type it points to — **not by raw bytes**. This enables efficient array traversal without index variables.

```
Address:  1000    1004    1008    1012    1016
Array:  [  10  ][  20  ][  30  ][  40  ][  50  ]
          ^p

p++        ->  p now points to 1004  (jumped sizeof(int) = 4 bytes)
p + 2      ->  points to 1008        (no side-effect, just calculates)
last-first ->  ptrdiff_t: number of elements between two pointers
```

---

### 3. Array vs Pointer — Critical Differences

These look similar but behave very differently in important situations:

```c
int arr[] = {1, 2, 3, 4, 5};   // ARRAY   - owns its memory
int *ptr  = arr;                 // POINTER - just points to arr[0]
```

|     Aspect    |       Array `int arr[5]`      |        Pointer `int *ptr`         |
|---------------|-------------------------------|-----------------------------------|
| `sizeof`      | 20 bytes (full array)         | 8 bytes (pointer only)            |
| Re-assign     | ❌ ILLEGAL — not an lvalue   | ✅ Legal — can point elsewhere    |
| Ownership     | Owns its storage              | Borrows someone else's            |
| `arr[i]`      | Sugar for `*(arr + i)`        | Same — `ptr[i] == *(ptr+i)`       |
| In function   | **Decays to pointer!**        | Already a pointer                 |

> ⚠️ **The `sizeof` trap:** When you pass an array to a function it becomes a pointer and you lose size information. Always pass the length separately.

> ⚠️ **The `&arr` type trap:** `&arr` and `ptr` have the same numeric address but different types. `&arr + 1` jumps `sizeof(arr)` = 20 bytes. `ptr + 1` jumps `sizeof(int)` = 4 bytes.

---

### 4. Pass-by-Pointer Pattern

C is **strictly pass-by-value**. Functions receive copies of their arguments. To mutate a caller's variable from inside a function, you must pass its address.

```c
// WRONG — modifies copies only, caller is unchanged
void bad_swap(int a, int b) {
    int t = a; a = b; b = t;
}

// CORRECT — operates on originals via their addresses
void swap_integers(int *a, int *b) {
    int t = *a; *a = *b; *b = t;
}

swap_integers(&x, &y);   // pass ADDRESSES, not values
```

---

## Demonstrations

Comment out any call in `main.c` to study a single topic in isolation.

### Pointer Basics

Shows declaration, address-of, dereference, write-through, double indirection, and the NULL pointer.

```c
int x = 42;
int *ptr = &x;       // ptr holds the address of x
*ptr = 100;          // x is now 100 - changed through the pointer

int **pptr = &ptr;   // pointer to pointer (double indirection)
**pptr == 100        // pptr -> ptr -> x
```

---

### Pointer Arithmetic

Walks an array forward and backward using only `p++` / `p--`, demonstrates offset notation, and shows `ptrdiff_t`.

```c
int arr[] = {10, 20, 30, 40, 50};
int *p = arr;

// Forward walk  - addresses increment by sizeof(int) = 4 each step
// Backward walk - same in reverse using p--
// Offset:    *(p + 2)          == 30
// Difference: &arr[4] - &arr[0] == 4  (elements, not bytes)
```

---

### Array vs Pointer

Side-by-side comparison of `sizeof`, address values, element access syntax, the `&arr` type trap, and the re-assignment restriction.

```c
sizeof(arr) = 20   // 5 elements × 4 bytes - full array
sizeof(ptr) =  8   // pointer variable only (64-bit machine)

&arr + 1           // jumps 20 bytes - moves past the ENTIRE array!
ptr  + 1           // jumps  4 bytes - moves to the next int
```

---

### Reverse Array

Two-pointer technique: head pointer at the start, tail pointer at the end - swap and converge until they meet.

```
Before: [ 10  20  30  40  50  60  70 ]
         ^head                  ^tail

Step 1: swap arr[0]=10  <->  arr[6]=70
Step 2: swap arr[1]=20  <->  arr[5]=60
Step 3: swap arr[2]=30  <->  arr[4]=50
                    ^meet — done

After:  [ 70  60  50  40  30  20  10 ]
```

---

### Swap Using Pointers

Demonstrates why pass-by-value fails and how passing addresses fixes it. Includes a self-swap guard.

```c
int x = 25, y = 75;
swap_integers(&x, &y);
// x == 75, y == 25

// Self-swap guard — prevents XOR-trick corruption:
if (a == b) return;
```

---

### Print via Pointer Increment

Traverses an array using a pointer and a one-past-the-end sentinel instead of an index variable.

```c
const int *p   = arr;        // start at first element
const int *end = arr + len;  // one-past-the-end sentinel

while (p < end) {
    printf("%d ", *p);
    p++;                     // advance by sizeof(int) bytes
}
```

> `const int *` — the pointer can move but it **cannot modify** the values it points to. Standard practice for read-only traversal in embedded firmware.

---

## Build Instructions

### Prerequisites

| GCC | 10.x or later |
| GNU Make | 4.x or later |

---

### Using Make (Recommended)

```bash
make          # compile all source files → produces ./pointers_demo
make run      # compile (if needed) then run immediately
make clean    # remove all .o files and the binary
```
---

**Compiler flags explained:**

|         Flag              |                     Purpose                           |
|---------------------------|-------------------------------------------------------|
| `-Wall -Wextra -Wpedantic`| Enable all warnings — this project produces **zero**  |
| `-std=c11`                | Enforce ISO C11 standard                              |
| `-Iinclude`               | Add `include/` to the header search path              |

## Running the Program

```bash
# Full build + run in one step:
make run

# Or run the already-compiled binary directly:
./pointers_demo
```

To study a single topic, open `main.c` and comment out the calls you do not need:

```c
int main(void)
{
    demo_pointer_basics();        // Topic 1 — declaration, &, *
    demo_pointer_arithmetic();    // Topic 2 — ++, --, offset, ptrdiff_t
    // demo_array_vs_pointer();   // Topic 3 — commented out
    demo_array_ops();             // Topics 4 & 6 — reverse + print
    demo_swap();                  // Topic 5 — swap via pointer
    return 0;
}
```

---

## Expected Output

> Memory addresses will differ on every machine and every run — that is normal. Values and logical relationships are always consistent.

```
 ------------------------------------------- 
|                 C POINTERS                |
 -------------------------------------------

==========================================
  Pointer Basics
==========================================
  Variable x          : value = 42
  Address of x (&x)   : 00000096447FFE3C
  Pointer ptr         : holds address 00000096447FFE3C
  Dereference (*ptr)  : value at that address = 42

  After *ptr = 100:
    x    = 100  (changed via pointer)
    *ptr = 100  (same object, same value)

  Double indirection:
    pptr   : 00000096447FFE30  (holds address of ptr)
    *pptr  : 00000096447FFE3C  (holds address of x)
    **pptr : 100  (value of x)

  NULL pointer value  : 0000000000000000
  (Dereferencing NULL is undefined behaviour -- never do it)

==========================================
   Pointer Arithmetic
==========================================
  sizeof(int)         = 4 bytes
  Base address (arr)  = 00000096447FFE10

  Walking forward with p++:
    p = 00000096447FFE10  |  *p = 10  (arr[0])
    p = 00000096447FFE14  |  *p = 20  (arr[1])
    p = 00000096447FFE18  |  *p = 30  (arr[2])
    p = 00000096447FFE1C  |  *p = 40  (arr[3])
    p = 00000096447FFE20  |  *p = 50  (arr[4])

  Walking backward with p--:
    p = 00000096447FFE20  |  *p = 50  (arr[4])
    p = 00000096447FFE1C  |  *p = 40  (arr[3])
    p = 00000096447FFE18  |  *p = 30  (arr[2])
    p = 00000096447FFE14  |  *p = 20  (arr[1])
    p = 00000096447FFE10  |  *p = 10  (arr[0])

  Offset notation (p + n):
    *(p + 0) = 10
    *(p + 1) = 20
    *(p + 2) = 30
    *(p + 3) = 40
    *(p + 4) = 50

  Pointer difference:
    last - first = 4  (number of int elements between them)

==========================================
  Array vs Pointer
==========================================

  [1] sizeof comparison:
    sizeof(arr)  = 20 bytes  <- entire array (5 elements x 4 bytes)
    sizeof(ptr)  = 8 bytes  <- just the pointer variable

  [2] Address equivalence (array name decays to &arr[0]):
    arr           = 00000096447FFE20
    &arr[0]       = 00000096447FFE20  (identical)
    ptr           = 00000096447FFE20  (same address, different variable)
    &arr          = 00000096447FFE20  (same address; type: int (*)[5])
    &arr + 1      = 00000096447FFE34  (jumps by sizeof(arr) = 20 bytes!)
    ptr  + 1      = 00000096447FFE24  (jumps by sizeof(int) = 4 bytes)

  [3] Equivalent element access syntax:
    arr[0]=1  *(arr+0)=1  ptr[0]=1  *(ptr+0)=1
    arr[1]=2  *(arr+1)=2  ptr[1]=2  *(ptr+1)=2
    arr[2]=3  *(arr+2)=3  ptr[2]=3  *(ptr+2)=3
    arr[3]=4  *(arr+3)=4  ptr[3]=4  *(ptr+3)=4
    arr[4]=5  *(arr+4)=5  ptr[4]=5  *(ptr+4)=5

  [4] Re-assignment:
    ptr re-pointed to 'other': ptr[0]=99, ptr[1]=88, ptr[2]=77
    arr = other;  <- would be a COMPILE ERROR (not an lvalue)

  [5] Array parameter decay rule:
    void fn(int arr[])  is IDENTICAL to  void fn(int *arr)
    Inside fn, sizeof(arr) == sizeof(ptr), NOT the full array.
    Always pass the length separately (or use a struct wrapper).

==========================================
  Print Array via Pointer Increment
==========================================

  Array 'data'  : [ 7 14 21 28 35 42 ]

  Trace of pointer walk:
    p = 00000096447FFDF0  |  *p =  7  (index 0)
    p = 00000096447FFDF4  |  *p = 14  (index 1)
    p = 00000096447FFDF8  |  *p = 21  (index 2)
    p = 00000096447FFDFC  |  *p = 28  (index 3)
    p = 00000096447FFE00  |  *p = 35  (index 4)
    p = 00000096447FFE04  |  *p = 42  (index 5)

==========================================
  Reverse Array Using Pointers
==========================================

  Before reversal : [ 10 20 30 40 50 60 70 ]

  Two-pointer reversal steps:
    Step 1: swap arr[0]=10  <->  arr[6]=70
    Step 2: swap arr[1]=20  <->  arr[5]=60
    Step 3: swap arr[2]=30  <->  arr[4]=50

  After  reversal : [ 70 60 50 40 30 20 10 ]

  Single-element reverse : [ 99 ]

==========================================
  Swap Using Pointers
==========================================

  Before swap: x = 25,  y = 75
  Calling swap_integers(&x, &y)...
  After  swap: x = 75,  y = 25

  Memory layout after swap:
    &x = 00000096447FFE4C  -> holds 75
    &y = 00000096447FFE48  -> holds 25

  Inside swap_integers(int *a, int *b):
    a points to x : *a was 25
    b points to y : *b was 75
    int tmp = *a  -> save original *a
    *a = *b       -> overwrite first with second
    *b = tmp      -> overwrite second with saved value

  Self-swap guard: swap_integers(&z, &z) -- should be no-op.
    z is still 42  (OK)

  Why pass-by-value fails:
    void bad_swap(int a, int b) modifies COPIES of x and y.
    The originals in the caller remain untouched.
    Always pass pointers when mutation of caller data is needed.

------------------------------------------
  All topics complete.
------------------------------------------

```
---

## Author

**Rohith Kalarikkal Ramakrishnan**  
Version: 1.0.0 
Language: C11

---
