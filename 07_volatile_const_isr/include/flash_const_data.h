/**
 * @file    flash_const_data.h
 * @brief   Demonstration of const-qualified data residing in Flash (ROM).
 *
 * WHY const FOR FLASH MEMORY?
 * ------------------------------------------------------------------
 * On MCUs (ARM Cortex-M, AVR, MSP430 …) memory is divided into:
 *
 *   Flash (ROM-like, non-volatile) - code + read-only data (.rodata section)
 *   SRAM  (volatile, fast)         - stack, heap, global variables (.data/.bss)
 *
 * SRAM is scarce (typically 8–256 KB). Flash is larger (64 KB – several MB).
 *
 * Declaring a variable const:
 *   1. Instructs the LINKER to place it in the .rodata section -> Flash.
 *   2. Prevents accidental writes (compiler error if code tries to modify it).
 *   3. Allows the compiler to share identical string literals.
 *   4. On Harvard-architecture CPUs (AVR) the programmer uses PROGMEM /
 *      pgm_read_byte() explicitly.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef FLASH_CONST_DATA_H
#define FLASH_CONST_DATA_H

#include "platform_types.h"

void const_flash(void);

/* ------------------------------------------------------------------
 * Section-placement macro
 *
 * In a real project the linker script defines a ".rodata" or custom ".flash"
 * output section. GCC's __attribute__((section(...))) pins the symbol there.
 * On a host build we just mark it const, the section attribute is a no-op.
 * ------------------------------------------------------------------ */
#ifdef __arm__
#  define FLASH_DATA   __attribute__((section(".rodata"))) const
#else
#  define FLASH_DATA   const   /* host simulation */
#endif

/* ------------------------------------------------------------------
 * Lookup table stored in Flash
 *
 * A sine table for a 12-bit DAC — 256 samples, quarter-wave, Q0.12 fixed
 * point (values 0 – 4095). Without const this 512-byte table would consume
 * precious SRAM on every power cycle.
 * ------------------------------------------------------------------ */
extern FLASH_DATA uint16_t g_sine_lut[256];

/* ------------------------------------------------------------------
 * Firmware version descriptor — immutable, lives in Flash
 * ------------------------------------------------------------------ */
typedef struct
{
    const uint8_t  major;
    const uint8_t  minor;
    const uint8_t  patch;
    const char    *build_date;   /* Points to string literal in Flash */
    const char    *build_time;
} FwVersion_t;

extern FLASH_DATA FwVersion_t g_fw_version;

/* ------------------------------------------------------------------
 * Error-code-to-string table (stored in Flash, not SRAM)
 * ------------------------------------------------------------------ */
typedef struct
{
    const uint8_t  code;
    const char    *description;  /**< Pointer to string literal in Flash */
} ErrorEntry_t;

extern FLASH_DATA ErrorEntry_t g_error_table[];
extern FLASH_DATA uint8_t      g_error_table_size;

/* ------------------------------------------------------------------
 * Read-only hardware register example
 *
 * Some peripheral registers are write-protected by hardware (e.g., a chip-ID
 * register). We model them as volatile const to prevent software writes while
 * still forcing every read to go to the actual register address.
 *
 *   volatile - do not cache; re-read every time (HW may change it)
 *   const    - software may not write it (it would be a bus error on HW)
 * ------------------------------------------------------------------ */
typedef struct __attribute__((packed))
{
    volatile const uint32_t CHIP_ID;    /* Offset 0x00 – Chip identifier (RO)  */
    volatile const uint32_t REVISION;   /* Offset 0x04 – Silicon revision (RO) */
    volatile const uint32_t UNIQUE_ID0; /* Offset 0x08 – 96-bit UID word 0     */
    volatile const uint32_t UNIQUE_ID1; /* Offset 0x0C – 96-bit UID word 1     */
    volatile const uint32_t UNIQUE_ID2; /* Offset 0x10 – 96-bit UID word 2     */
} DeviceID_RegMap_t;

extern DeviceID_RegMap_t *const SIM_DEVICE_ID;

/* ------------------------------------------------------------------
 * API
 * ------------------------------------------------------------------ */

/** @brief Initialise simulated Flash data and Device-ID registers. */
void flash_const_init(void);

/** @brief Print firmware version from Flash. */
void flash_print_fw_version(void);

/** @brief Demonstrate sine LUT access. */
void flash_sine_lut(void);

/** @brief Demonstrate read-only register access. */
void flash_device_id(void);

/** @brief Look up an error string from Flash table. */
const char *flash_get_error_str(uint8_t code);

#endif /* FLASH_CONST_DATA_H */
