/**
 * @file    system_config.c
 * @brief   DEFINITIONS of all system-wide global variables.
 *
 *
 * GLOBAL vs EXTERN — Quick Reference
 *  ----------------------------------------------------------------------
 * |  Term      │ Storage  │ Visible to other files?  │  Keyword needed?  |
 * |----------------------------------------------------------------------|
 * |  global    │ YES (BSS │ YES, if declared extern  │   none (default)  |
 * |            │ or DATA) │ in a header              │                   |
 * |----------------------------------------------------------------------|
 * |  extern    │ NO       │ References existing def  │   extern          |
 * |----------------------------------------------------------------------|
 * |  static    │ YES      │ NO — file scope only     │   static          |
 *  ----------------------------------------------------------------------
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "system_config.h"

/* =========================================================================
 * GLOBAL VARIABLE DEFINITIONS
 *
 * These lines allocate actual storage (RAM on embedded targets).
 * ========================================================================= */

/**
 * System tick counter.
 * 'volatile' is mandatory for variables modified by ISRs or hardware.
 * It prevents the compiler from caching the value in a register.
 */
volatile uint32_t g_system_tick_ms = 0U;

/**
 * System initialisation flag.
 * Starts as false. Set to true by the startup sequence in main().
 */
bool g_system_initialised = false;

/**
 * Read-only system name stored in flash (on embedded targets the linker
 * places 'const' string literals in the .rodata section automatically).
 */
const char * const g_system_name = "EmbeddedDemo-v1.0";

/**
 * Global error counter.
 * Starts at zero. Any module may call INCREMENT_ERROR_COUNT() to bump it.
 */
uint32_t g_error_count = 0U;
