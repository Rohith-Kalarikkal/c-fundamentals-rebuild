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

> A structured, well-commented C project demonstrating core pointer concepts used in professional embedded / firmware development. Each concept lives in its own module so you can study, compile, and run them independently.
The code reflects real-world firmware patterns: sensor device handles, peripheral driver callbacks, lookup-table state machines, and safe dynamic memory lifecycle management.

## Project Structure

```
04_pointers_level2/
├── include/
│   ├── double_ptr.h       # Double pointer API declarations
│   ├── struct_ptr.h       # SensorNode_t type + struct pointer API
│   └── func_ptr.h         # Function pointer types + calculator API
├── src/
│   ├── main.c             # Entry point - orchestrates all demos
│   ├── double_ptr.c       # double pointer basics + 2D matrix
│   ├── struct_ptr.c       # struct pointer on stack and heap
│   └── func_ptr.c         # function pointers + calculator + callbacks
├── .gitignore
├── Makefile
└── README.md
```

---

## Topics Covered


|  # | Topic | File |
|---|-------|------|
| 1 | Double pointer basics - `**ptr`, out-parameter alloc | `double_ptr.c` |
| 2 | Dynamic 2D matrix using double pointers | `double_ptr.c` |
| 3 | Pointer to struct - `->` operator, stack allocation | `struct_ptr.c` |
| 4 | Pointer to struct - `malloc` / `memset` / `free` lifecycle | `struct_ptr.c` |
| 5 | Function pointer declaration, assignment, array iteration | `func_ptr.c` |
| 6 | Function pointer calculator - lookup-table design | `func_ptr.c` |
| 7 | Function pointer inside a struct - strategy pattern | `func_ptr.c` |
| 8 | Event callback pattern - simulated peripheral driver | `func_ptr.c` |

---

## Key Concepts

### Double Pointer (`**ptr`)
A double pointer stores the address of another pointer. It allows a function to modify the caller's pointer variable directly — the standard pattern for out-parameters and linked-list head manipulation.

```c
int   value = 42;
int  *ptr   = &value;   // ptr  → value
int **dptr  = &ptr;     // dptr → ptr → value

**dptr = 100;           // modifies value through two levels of indirection
*dptr  = &other;        // redirects ptr to point at a different variable
```

---

### Pointer to Struct
Passing a struct by pointer avoids copying the entire object onto the stack. The arrow operator `->` dereferences the pointer and accesses a member in one step.

```c
SensorNode_t  node  = { .sensor_id = 1, .temperature = 23.5f };
SensorNode_t *p     = &node;

p->temperature = 37.8f;        // arrow operator — equivalent to (*p).temperature
printf("%f", p->temperature);
```

---

### Function Pointer
A function pointer stores the address of a function. The pointed-to function can be swapped at runtime, enabling callbacks, lookup tables, and strategy patterns without any `if/else` chains.

```c
typedef double (*ArithmeticOp_t)(double a, double b);

double add(double a, double b) { return a + b; }

ArithmeticOp_t op = add;    // assign
double result = op(3, 4);   // call — result = 7.0
```

---

### Dynamic Memory (`malloc` / `free`)
Heap allocation allows objects to be created at runtime and outlive the function that created them. Every `malloc` must be paired with a `free`, and the pointer must be set to `NULL` after freeing to prevent dangling-pointer bugs.

```c
SensorNode_t *sensor = (SensorNode_t *)malloc(sizeof(SensorNode_t));
if (sensor == NULL) { /* handle allocation failure */ }

memset(sensor, 0, sizeof(SensorNode_t));   // zero-init — no garbage values
sensor->sensor_id = 101;

free(sensor);
sensor = NULL;    // prevent use-after-free
```

---

## Demonstrations

### Double Pointer Basics
Illustrates the three-level memory chain (`value → ptr → dptr`). Shows modifying the underlying value through `**dptr`, redirecting what `ptr` points to via `*dptr`, and the out-parameter pattern where a helper function allocates memory and delivers it back to the caller through a `int **` argument.

---

### Dynamic 2D Matrix
Allocates a fully dynamic `rows × cols` integer matrix using `int **`. Demonstrates the correct two-phase allocation (array of row pointers first, then each row's column buffer), fills it with sequential values, prints it, and frees all memory in reverse order to avoid leaks.

---

### Struct Pointer (Stack)
Creates a `SensorNode_t` (52 bytes, containing a nested `GpsCoord_t`) on the stack and obtains a pointer to it. Passes the pointer to `sensor_node_print` and `sensor_node_update`, demonstrating that functions can modify the original struct without copying it. Shows `const SensorNode_t *` for read-only access vs `SensorNode_t *` for mutation.

---

### Struct Pointer (Heap / malloc)
Full `malloc` -> `memset` -> populate -> use -> `free` lifecycle for a heap-allocated `SensorNode_t`. The function returns the pointer to `main`, which updates and then frees it - modelling the "create / use / destroy" device handle pattern common in embedded middleware and HAL layers.

---

### Function Pointer Basics
Declares an `ArithmeticOp_t` function pointer variable, assigns `op_add` then `op_mul` to it, and calls it at the same call site both times. Also populates a 4-element array of function pointers and iterates over it - demonstrating that function pointers behave like any other pointer type.

---

### Lookup-Table Calculator
A `CalcEntry_t` table maps operator symbols (`+`, `-`, `*`, `/`) to their function pointer implementations. `demo_calculator` searches the table by symbol and calls `entry.fn(a, b)` through the pointer. Adding a new operator requires only one new row in the table — no `switch` or `if/else` to touch. Edge case: divide-by-zero is caught and reported gracefully.

---

### Function Pointer in Struct
Defines a `Widget_t` struct with `print` and `transform` function pointers as members — C's equivalent of object methods. Two widgets are created with different print styles (`verbose` vs `compact`) and different transform strategies (`double` vs `square`). The strategy is swapped at runtime by simply reassigning the pointer, demonstrating polymorphism without any OOP language features.

---

### Event Callback Pattern
Registers `my_event_handler` as an `EventCallback_t` and passes it to `simulated_driver_fire_event`, which invokes it for three different event codes. The driver holds no knowledge of what the callback does — it just calls whatever pointer was registered. This is the exact pattern used for GPIO interrupt handlers, UART receive notifications, and DMA completion callbacks in real firmware. Also tests the `NULL` callback edge case.

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

---

## Expected Output

```
 ---------------------------------------------------------
      Embedded C  -  Pointer Concepts Reference
 ---------------------------------------------------------

 ------------------------------------------
            Double Pointer Basics
 ------------------------------------------
  value        = 100
  *ptr         = 100  (via single pointer)
  **dptr       = 100  (via double pointer)
  &value       = 000000EE123FFBA4
  ptr          = 000000EE123FFBA4  (ptr stores address of value)
  *dptr        = 000000EE123FFBA4  (dptr stores address of ptr)

  After **dptr = 200:
  value = 200  (original variable modified via **dptr)

  After *dptr = &another (redirect ptr):
  *ptr  = 999  (ptr now points to 'another')
  value = 200  (unchanged)

  Out-param alloc: *heap_int = 42  (addr=00000204941E9850)
  Memory freed; heap_int set to NULL.

 ------------------------------------------
           2-D Matrix via Double Ptr
 ------------------------------------------
  Allocating 3 x 4 matrix...
  Matrix contents:
    Row 0:    0   1   2   3
    Row 1:    4   5   6   7
    Row 2:    8   9  10  11
  All matrix memory freed cleanly.

--------------------------------------------
             Struct Pointer (Stack)
--------------------------------------------
  Initial state (access via -> operator):
   ----- SensorNode --------------------------------
    ID          : 101
    Name        : TempSensor_A
    Temperature : 23.50 Celsuis
    Status      : ACTIVE
    Location    : (51.5074, -0.1278)
   -------------------------------------------------

  After sensor_node_update (temp=37.8, active=0):
   ----- SensorNode --------------------------------
    ID          : 101
    Name        : TempSensor_A
    Temperature : 37.80 Celsuis
    Status      : FAULTED
    Location    : (51.5074, -0.1278)
   -------------------------------------------------

  sizeof(SensorNode_t) = 52 bytes  (pointer avoids copying all of this onto the stack)

--------------------------------------------
         Struct Pointer (Heap/malloc)
--------------------------------------------
  Heap-allocated sensor at address: 00000204941E8370
   ----- SensorNode --------------------------------
    ID          : 202
    Name        : CryoSensor_B
    Temperature : -5.30 Celsuis
    Status      : ACTIVE
    Location    : (48.8566, 2.3522)
   -------------------------------------------------

  Returning heap pointer to caller for further use...

  [main] Updating heap sensor then freeing it.
   ----- SensorNode --------------------------------
    ID          : 202
    Name        : CryoSensor_B
    Temperature : 0.00 Celsuis
    Status      : FAULTED
    Location    : (48.8566, 2.3522)
   -------------------------------------------------
  [main] heap_sensor freed and set to NULL.

 ------------------------------------------
          Function Pointer Basics
 ------------------------------------------
  operation = op_add  ->  operation(10, 3) = 13
  operation = op_mul  ->  operation(10, 3) = 30

  Iterating over function pointer array (a=12, b=4):
    add  : 16.00
    sub  : 8.00
    mul  : 48.00
    div  : 3.00

 -------------------------------------------
          Function Ptr Calculator
 -------------------------------------------
  Expression: 15.00  +  7.00
  Operation : add
  Result    : 22.0000

 -------------------------------------------
          Function Ptr Calculator
 -------------------------------------------
  Expression: 15.00  /  4.00
  Operation : divide
  Result    : 3.7500

 -------------------------------------------
          Function Ptr Calculator
 -------------------------------------------
  Expression: 9.00  *  3.00
  Operation : multiply
  Result    : 27.0000

 -------------------------------------------
          Function Ptr Calculator
 -------------------------------------------
  Expression: 5.00  /  0.00
[ERROR] Division by zero attempted.
  Operation : divide
  Result    : 0.0000

--------------------------------------------
         Function Pointer in Struct
--------------------------------------------
  Before transform:
  [VERBOSE] Widget 'Counter' holds value: 5
  Sensor=7

  After transform (w1=double, w2=square):
  [VERBOSE] Widget 'Counter' holds value: 10
  Sensor=49

  Swapping w1 strategy to 'square' at runtime:
  [VERBOSE] Widget 'Counter' holds value: 9

--------------------------------------------
           Event Callback Pattern
--------------------------------------------
  Firing simulated peripheral events:
  [CALLBACK] Event=0x01  Context="MainApp"  -> Button pressed!
  [CALLBACK] Event=0x02  Context="MainApp"  -> Sensor data ready.
  [CALLBACK] Event=0x03  Context="MainApp"  -> Communication error!

  Firing event with NULL callback (should log error):
[ERROR] No callback registered event 0x01 dropped.

 ------------------------------------------
          Completed successfully.
 ------------------------------------------

```
---

## Author

**Rohith Kalarikkal Ramakrishnan**  
Version: 1.0.0 
Language: C11

---
