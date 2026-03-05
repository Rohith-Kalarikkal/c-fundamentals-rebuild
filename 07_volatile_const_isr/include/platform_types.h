/**
 * @file    platform_types.h
 * @brief   Platform-specific type definitions for embedded targets.
 *
 * In a real MNC project (e.g., Bosch, Qualcomm, TI), this file would be
 * generated or supplied by the BSP (Board Support Package). We define it
 * manually here so the program compiles on any host (Linux/Windows/Mac) while
 * remaining representative of real embedded practice.
 *
 * Coding standard  : MISRA-C 2012 inspired
 * Target arch      : Generic 32-bit (ARM Cortex-M representation)
 * Compiler         : GCC / Clang / ARMCC
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

/* --------------------------------------------------------------------------
 * Standard fixed-width types (C99).
 * On a bare-metal target without <stdint.h>, these would be defined manually.
 * -------------------------------------------------------------------------- */
#include <stdint.h>   /* uint8_t, uint16_t, uint32_t … */
#include <stdbool.h>  /* bool, true, false               */
#include <stddef.h>   /* size_t, NULL                    */

/* --------------------------------------------------------------------------
 * Convenience aliases used across the codebase
 * -------------------------------------------------------------------------- */
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;

/* -------------------------------------------------------------------------- 
 * Register-access macros
 *
 * REG32(addr)  — dereference a memory-mapped 32-bit hardware register.
 *               The cast chain:
 *                 (uintptr_t)(addr)          - treat address as integer
 *                 (volatile uint32_t *)      - pointer to volatile HW cell
 *                 *(...)                     - read/write the actual cell
 *
-------------------------------------------------------------------------- */
#define REG32(addr)   ( *(volatile uint32_t *)(uintptr_t)(addr) )
#define REG16(addr)   ( *(volatile uint16_t *)(uintptr_t)(addr) )
#define REG8(addr)    ( *(volatile uint8_t  *)(uintptr_t)(addr) )

/* --------------------------------------------------------------------------
 * Bit-manipulation helpers (named after ARM CMSIS style)
 * -------------------------------------------------------------------------- */
#define BIT(n)              ( 1UL << (n) )
#define SET_BIT(reg, bit)   ( (reg) |=  (bit) )
#define CLR_BIT(reg, bit)   ( (reg) &= (uint32_t)(~(uint32_t)(bit)) )
#define TST_BIT(reg, bit)   ( ((reg) & (bit)) != 0U )

/* --------------------------------------------------------------------------
 * Compiler hints
 * -------------------------------------------------------------------------- */
/** Force the compiler to NOT optimise away a memory access. */
#define MEMORY_BARRIER()    __asm__ __volatile__ ("" ::: "memory")

/** Mark a function as an interrupt service routine (GCC syntax). */
#define ISR_ATTR            __attribute__((interrupt))

/* --------------------------------------------------------------------------
 * Return-code type — used by all API functions
 * -------------------------------------------------------------------------- */
typedef enum
{
    STATUS_OK    = 0,   /* Operation succeeded.          */
    STATUS_ERROR = 1,   /* Generic error.                */
    STATUS_BUSY  = 2,   /* Peripheral is busy.           */
    STATUS_TIMEOUT = 3  /* Operation timed out.          */
} status_t;

#endif /* PLATFORM_TYPES_H */
