/**
 * @file    system_config.h
 * @brief   System-wide shared variables and constants.
 *
 * RULE OF THUMB (MNC coding standard):
 *   - A global variable is DEFINED in exactly ONE .c file.
 *   - Every other translation unit that needs it uses an EXTERN declaration,
 *     which is placed here in the shared header.
 *   - Never put a variable *definition* (without extern) in a header file;
 *     doing so would create a separate copy in every .c that includes it,
 *     causing linker "multiple definition" errors or, worse, silent ODR
 *     (One Definition Rule) violations.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef SYSTEM_CONFIG_H   /* Header guard - prevents double inclusion */
#define SYSTEM_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

/* =========================================================================
 * COMPILE-TIME CONSTANTS
 * Use #define or 'const' for values that never change at runtime.
 * Prefer typed constants (const) over bare #define for better type safety.
 * ========================================================================= */

#define FIRMWARE_VERSION_MAJOR  (1U)
#define FIRMWARE_VERSION_MINOR  (0U)

/** Maximum number of sensor readings stored in the ring buffer. */
#define SENSOR_BUFFER_SIZE      (16U)

/* =========================================================================
 * EXTERN DECLARATIONS — global variables DEFINED in system_config.c
 *
 * The 'extern' keyword tells the compiler:
 *   "This variable EXISTS somewhere; do not allocate storage here.
 *    The linker will resolve the address at link time."
 *
 * Without 'extern', the compiler would allocate a NEW variable in every
 * translation unit that includes this header.
 * ========================================================================= */

/**
 * @brief System-wide operational tick counter, incremented every 1 ms.
 *        Defined in system_config.c and shared read/write across all modules.
 */
extern volatile uint32_t g_system_tick_ms;

/**
 * @brief Global flag indicating whether the system has been initialised.
 *        Modules must check this before performing any operation.
 *        Defined in system_config.c.
 */
extern bool g_system_initialised;

/**
 * @brief Read-only system name string.
 *        'const' here means the pointer itself is constant.
 *        Defined in system_config.c.
 */
extern const char * const g_system_name;

/**
 * @brief Running count of total errors detected across all modules.
 *        Defined in system_config.c and written by any module, read by main.
 */
extern uint32_t g_error_count;

/* =========================================================================
 * INLINE UTILITY MACRO
 * ========================================================================= */

/** Safe increment with overflow protection (wraps at UINT32_MAX). */
#define INCREMENT_ERROR_COUNT()  \
    do { if (g_error_count < UINT32_MAX) { g_error_count++; } } while (0)

#endif /* SYSTEM_CONFIG_H */
