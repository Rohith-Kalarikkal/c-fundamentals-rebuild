/**
 * @file    gpio_edge_counter.c
 *
 * @brief   GPIO rising-edge counter — demonstrates static locals in ISR context.
 *
 * @details This module maintains all its state in static local variables:
 *
 *          s_prev_state        — previous pin sample (for edge detection).
 *          s_rising_edge_count — accumulated rising-edge counter.
 *
 *          Neither variable is visible outside this file, yet both persist
 *          for the entire program lifetime — exactly the behavior needed for
 *          an interrupt-driven edge counter without polluting the global
 *          namespace.
 *
 *          In real firmware GPIO_EdgeCounter_Sample() would be placed inside
 *          an EXTI ISR, e.g.:
 *            void EXTI0_IRQHandler(void) { GPIO_EdgeCounter_Sample(HAL_GPIO_ReadPin(...)); }
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdint.h>

#include "gpio_edge_counter.h"

/* ============================================================================
 * Module-private state
 * ========================================================================== */

/**
 * Previous pin sample - file-scope static so it is shared between
 * GPIO_EdgeCounter_Sample() and GPIO_EdgeCounter_Reset() without
 * being visible outside this module.
 *
 * Initialized to GPIO_PIN_LOW (0) by the C runtime (BSS segment).
 */
static GpioPinState_t s_prev_state        = GPIO_PIN_LOW;

/**
 * Accumulated rising-edge count.
 * Placed at file scope so GPIO_EdgeCounter_GetCount() and
 * GPIO_EdgeCounter_Reset() can access it cleanly.
 */
static uint32_t       s_rising_edge_count = 0U;

/* ============================================================================
 * Public function implementations
 * ========================================================================== */

/**
 * @brief   Process one GPIO pin sample; detect and count rising edges.
 *
 * @details A rising edge is defined as a LOW -> HIGH transition between the
 *          previous sample and the current sample. Any other transition
 *          (LOW -> LOW, HIGH -> HIGH, HIGH -> LOW) is ignored for counting purposes.
 *
 *          s_prev_state is updated on every call so the next invocation
 *          always compares against the most recently seen level.
 *
 * @param   pin_state   Current sampled logic level of the GPIO pin.
 */
void GPIO_EdgeCounter_Sample(GpioPinState_t pin_state)
{
    /* Detect LOW → HIGH transition (rising edge) */
    if ((s_prev_state == GPIO_PIN_LOW) && (pin_state == GPIO_PIN_HIGH))
    {
        s_rising_edge_count++;
        printf("    Rising edge #%u detected  (prev=LOW, curr=HIGH)\n",
               s_rising_edge_count);
    }
    else
    {
        printf("    No edge              (prev=%s, curr=%s)\n",
               (s_prev_state == GPIO_PIN_LOW) ? "LOW " : "HIGH",
               (pin_state    == GPIO_PIN_LOW) ? "LOW " : "HIGH");
    }

    /* Always latch the current level for the next comparison */
    s_prev_state = pin_state;
}

/* -------------------------------------------------------------------------- */

/**
 * @brief   Return the accumulated rising-edge count.
 */
uint32_t GPIO_EdgeCounter_GetCount(void)
{
    return s_rising_edge_count;
}

/* -------------------------------------------------------------------------- */

/**
 * @brief   Reset the rising-edge counter to zero.
 *
 * @note    s_prev_state is intentionally NOT reset - the module must
 *          continue to track the physical pin level correctly even after
 *          the counter is cleared (e.g., for a periodic report).
 */
void GPIO_EdgeCounter_Reset(void)
{
    s_rising_edge_count = 0U;
}

/* End of file — gpio_edge_counter.c */
