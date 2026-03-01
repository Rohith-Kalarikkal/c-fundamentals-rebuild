/**
 * @file    app_types.h
 *
 * @brief   Project-wide type definitions shared across all modules.
 *
 * @details Centralizing type definitions in one header prevents duplicate
 *          declarations, circular include issues, and type mismatches between
 *          modules. Every module that needs these types includes only this file.
 *
 *          This header must have ZERO includes of project headers.
 *          It may only include C standard library type headers (stdint, stdbool).
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * GPIO types
 * ========================================================================== */

/**
 * @brief GPIO pin logic-level abstraction.
 *
 * Mirrors the pin-state type in a real HAL (e.g., STM32 HAL GPIO_PinState).
 * Using a named enum instead of a raw integer prevents accidentally passing
 * an arbitrary integer where a pin state is expected.
 */
typedef enum
{
    GPIO_PIN_LOW  = 0U,   /**< Pin is at logic LOW  (0 V) */
    GPIO_PIN_HIGH = 1U    /**< Pin is at logic HIGH (VCC) */
} GpioPinState_t;

/* ============================================================================
 * FSM types
 * ========================================================================== */

/**
 * @brief States for the demonstration finite state machine.
 *
 */
typedef enum
{
    FSM_STATE_IDLE      = 0U,   /**< System idle, awaiting command     */
    FSM_STATE_RUNNING   = 1U,   /**< Normal processing in progress     */
    FSM_STATE_FAULT     = 2U    /**< Fault detected — safe-state only  */
} FsmState_t;

#endif /* APP_TYPES_H */

/* End of file — app_types.h */
