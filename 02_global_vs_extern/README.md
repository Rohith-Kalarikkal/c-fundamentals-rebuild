# Global vs Extern in C — Demonstration Project

> A modular C project demonstrating professional memory management, encapsulation, and defensive
  programming techniques. This project simulates a simple Sensor -> Controller -> Actuator loop typical in embedded systems.

 ## Table of Contents
 
 - [Project Structure](#project-structure)
 - [Key Concepts](#key-concepts)
 - [Build Instructions](#build-instructions)
 - [Running the Program](#running-the-program)
 - [Expected Output](#expected-output)
 - [Coding Standards](#coding-standards)
 - [Testing Logic Errors](#testing-logic-errors)
 - [Author](#author) 

## Project Structure

```
global_vs_extern/
├── README.md           # This file
├── Makefile            # Build system
├── main.c              # Entry point. ties all modules together
├── system_config.h     # Global variable DECLARATIONS (extern) & constants
├── system_config.c     # Global variable DEFINITIONS (single source of truth)
├── sensor.h            # Sensor module interface
├── sensor.c            # Sensor module implementation
├── actuator.h          # Actuator module interface
└── actuator.c          # Actuator module implementation
```

## Key Concepts

### 1. Global & Extern (Shared Memory)

***Definition***: `g_system_tick_ms` is defined in `system_config.c`. This allocates actual RAM.
***Declaration***: The extern keyword in `system_config.h` allows `main.c`, `sensor.c`, and `actuator.c` to share the exact same memory address.
***Volatile***: Used for the system tick to ensure the compiler always reads the latest value from RAM (essential for variables updated by hardware timers or ISRs).

### 2. Static (Private Memory)

Variables like `s_read_count` (in `sensor.c`) and `s_current_level` (in `actuator.c`) are marked static.

This prevents other files from accessing them directly, ensuring Encapsulation. Access is only allowed through "Getter" functions like `Sensor_GetReadCount()`.

### 3. Defensive Programming

The code is designed to catch logic errors at runtime:

***Initialization Guards***: Modules check if the system is "ON" before starting.
***Boundary Checks***: The Actuator rejects any drive level above 100%.
***Null Pointer Protection***: The Sensor module validates pointers before writing data.

---

## Build Instructions

### Prerequisites

| GCC | 10.x or later |
| GNU Make | 4.x or later |

### Release Build

```bash
make
```
Produces: `/app`  
Flags: `-std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wmissing-prototypes -g3 -O0`

### Clean

```bash
make clean  # clean up
```

Removes the entire generated files.

---

## Running the Program 

```bash
make run
```
```bash
./app
```

---

## Expected Output

```
========================================================
  Global vs Extern - Embedded C Demonstration
  System: EmbeddedDemo-v1.0
  FW:     v1.0
========================================================

[MAIN] System startup...
[MAIN] g_system_initialised set to TRUE (shared via extern across all modules).

[SENSOR] Initialised successfully.
[ACTUATOR] Initialised successfully.

--- Control Loop (5 cycles) ---

[MAIN] -- Cycle 1 --
[SENSOR] Read #1 -> raw value = 1967  (tick = 10 ms)
[ACTUATOR] Level set to 48%  (tick = 10 ms)

[MAIN] -- Cycle 2 --
[SENSOR] Read #2 -> raw value = 2407  (tick = 20 ms)
[ACTUATOR] Level set to 58%  (tick = 20 ms)

[MAIN] -- Cycle 3 --
[SENSOR] Read #3 -> raw value = 3555  (tick = 30 ms)
[ACTUATOR] Level set to 86%  (tick = 30 ms)

[MAIN] -- Cycle 4 --
[SENSOR] Read #4 -> raw value = 3062  (tick = 40 ms)
[ACTUATOR] Level set to 74%  (tick = 40 ms)

[MAIN] -- Cycle 5 --
[SENSOR] Read #5 -> raw value = 1498  (tick = 50 ms)
[ACTUATOR] Level set to 36%  (tick = 50 ms)

--- Diagnostics ---
  Total sensor reads : 5
  Actuator active    : YES
  Final tick         : 50 ms
  Total errors       : 0

[MAIN] Shutting down. Goodbye.
========================================================
```

---

## Coding Standards

This project targets compliance with the following standards:

- **Language:** C11 (`-std=c11`)
- **MISRA-C 2012** alignment:
  - Rule 14.4 - Using `bool` for logical tests rather than integers.
  - Rule 10.3 - The use of the `U` suffix (e.g., 100U, 0U) to explicitly define constants as `Unsigned`.
    This prevents dangerous implicit sign conversions.
  - Rule 8.7 - Using `static` for variables and functions that don't need to be global (Internal Linkage).
  - Rule 8.5 - No object definitions in header files (using `extern` in headers and defining in .c files only).

---

## Testing Logic Errors

### Initialisation Error

**The Logic**: You try to use the sensor or actuator before the system is actually powered on and initialized.

**How to create it**: In `main.c`, comment out the call to `System_Startup()`.
```c
// System_Startup();  <-- Skip this!
sensor_status = Sensor_Init();
```

**What happens**: 
1. `Sensor_Init` checks the **extern** `g_system_initialised`.
2. Since it is still `false`, it prints: `[SENSOR] ERROR: System not initialised`.
3. It calls the macro to increment `g_error_count`.

**Result**: The program safely halts instead of trying to talk to unpowered hardware.

### Validation Error

**The Logic**: You try to drive the actuator beyond its physical limit (100%).

**How to create it**: In the `main.c` control loop, manually force a high value.
```C
// Inside the for loop in main.c
drive_level = 150; 
actuator_status = Actuator_SetLevel(drive_level);
```

**What happens**:
1. `Actuator_SetLevel` checks the `level` parameter.
2. It sees `150 > 100`.
3. It prints: `[ACTUATOR] ERROR: Level 150 exceeds maximum (100)`.
4. It increments the **global** `g_error_count`.

**Result**: The **static** `s_current_level` inside `actuator.c` remains unchanged (safe).

### Null Pointer Error

**The Logic**: You try to read the sensor but don't provide a place to store the data (a NULL pointer).

**How to create it**: In `main.c`, pass `NULL` to the sensor read function.
```C
// Inside the for loop in main.c
sensor_status = Sensor_Read(NULL); 
```

**What happens**:
1. `Sensor_Read` checks if `p_value == NULL`.
2. It prints: `[SENSOR] ERROR: NULL pointer passed to Sensor_Read()`.
3. It increments the **global** error counter.

**Result**: The program avoids a "Segmentation Fault" (crash) because it caught the bad pointer before trying to use it.

---

## Author

**Rohith Kalarikkal Ramakrishnan**  
Version: 1.0.0 
Language: C11

---
