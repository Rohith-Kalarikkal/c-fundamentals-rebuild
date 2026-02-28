/**
 * @file    gpio_edge_counter.h
 *
 * @brief   Public API for the GPIO rising-edge counter module.
 *
 * @details In production firmware this module would be registered as an EXTI
 *          interrupt handler. The static locals inside the .c file maintain
 *          all required state without any global variables, keeping the
 *          module fully encapsulated and easy to unit-test in isolation.
 *
 *          Caller contract:
 *            - Call GPIO_EdgeCounter_Sample() on every ISR firing (or tick).
 *            - Call GPIO_EdgeCounter_GetCount() to read accumulated edges.
 *            - Call GPIO_EdgeCounter_Reset() to clear the counter (e.g., on
 *              a watchdog timeout or periodic report).
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef GPIO_EDGE_COUNTER_H
#define GPIO_EDGE_COUNTER_H

#include <stdint.h>
#include "app_types.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief   Process one GPIO pin sample; detect and count rising edges.
 *
 * @details Internally retains the previous pin state in a static local so
 *          it can detect LOW->HIGH transitions across consecutive calls.
 *          No external state storage is required by the caller.
 *
 * @param   pin_state   Current sampled state of the GPIO pin.
 */
void GPIO_EdgeCounter_Sample(GpioPinState_t pin_state);

/**
 * @brief   Return the accumulated rising-edge count.
 *
 * @return  uint32_t  Total rising edges detected since startup (or last reset).
 */
uint32_t GPIO_EdgeCounter_GetCount(void);

/**
 * @brief   Reset the rising-edge counter to zero.
 *
 * @note    Does NOT reset the previous-state register; the next sample
 *          will still compare correctly against the last seen pin level.
 */
void GPIO_EdgeCounter_Reset(void);

#endif /* GPIO_EDGE_COUNTER_H */

/* End of file — gpio_edge_counter.h */
