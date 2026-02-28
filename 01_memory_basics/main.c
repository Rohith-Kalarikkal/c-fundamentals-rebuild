/**
 * @file    main.c
 *
 * @brief   Program entry point — orchestrates all demonstrations.
 *
 * @details main() is intentionally thin: it contains ZERO business logic.
 *          Its sole responsibilities are:
 *            1. Print the banner and section headers (via console_utils).
 *            2. Drive each demonstration module in sequence.
 *            3. Return EXIT_SUCCESS.
 *
 *          This separation ensures that every module can be compiled,
 *          linked, and unit-tested independently without pulling in main().
 *
 * @note    Build:
 *            make          — builds the demo binary
 *            make clean    — removes build artefacts
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Project headers */
#include "app_config.h"
#include "app_types.h"
#include "console_utils.h"
#include "var_demo.h"
#include "gpio_edge_counter.h"
#include "fsm.h"
#include "peripheral_init.h"

/* ============================================================================
 * Entry point
 * ========================================================================== */

int main(void)
{
    Console_PrintBanner();

    /* -----------------------------------------------------------------------
     * SECTION 1 — Core concept: local vs static local
     * --------------------------------------------------------------------- */
    Console_PrintSeparator("1. Local vs Static - Core Behavior");

    printf("\n  Calling VarDemo_RunLocal()  %u times:\n\n", DEMO_ITERATIONS);
    for (uint32_t i = 0U; i < DEMO_ITERATIONS; i++)
    {
        VarDemo_RunLocal();     /* counter ALWAYS prints 1 */
    }

    printf("\n  Calling VarDemo_RunStatic() %u times:\n\n", DEMO_ITERATIONS);
    for (uint32_t i = 0U; i < DEMO_ITERATIONS; i++)
    {
        VarDemo_RunStatic();    /* counter accumulates: 1, 2, 3 … */
    }

    printf("\n  Observation:\n");
    printf("    Local  -> counter RESETS  to 0 on every call  (always prints 1)\n");
    printf("    Static -> counter PERSISTS across calls        (prints 1..%u)\n",
           DEMO_ITERATIONS);

    /* -----------------------------------------------------------------------
     * SECTION 2 — Memory address comparison
     * --------------------------------------------------------------------- */
    Console_PrintSeparator("2. Memory Address Comparison (Stack vs Data Segment)");

    printf("\n  Calling VarDemo_RunAddressComparison() twice:\n\n");
    printf("  -- Call 1 --\n");
    VarDemo_RunAddressComparison();
    printf("\n  -- Call 2 --\n");
    VarDemo_RunAddressComparison();

    printf("\n  Observation:\n");
    printf("    Local  address CHANGES between calls (stack frame reallocated).\n");
    printf("    Static address is IDENTICAL  - same fixed location every time.\n");
    printf("    NEVER return a pointer to a local variable - it becomes a\n");
    printf("    dangling pointer the moment the function returns!\n");

    /* -----------------------------------------------------------------------
     * SECTION 3 - ISR rising-edge counter
     * --------------------------------------------------------------------- */
    Console_PrintSeparator("3. Embedded Use-Case: ISR Rising-Edge Counter");

    printf("\n  Simulated GPIO pin sequence: LOW, LOW, HIGH, LOW, HIGH, HIGH\n\n");

    /*
     * Simulated GPIO waveform: _ _ ‾ _ ‾ ‾
     * In real firmware these samples would come from HAL_GPIO_ReadPin() inside
     * an EXTI ISR - here we inject them deterministically for demonstration.
     */
    const GpioPinState_t waveform[] =
    {
        GPIO_PIN_LOW,
        GPIO_PIN_LOW,
        GPIO_PIN_HIGH,
        GPIO_PIN_LOW,
        GPIO_PIN_HIGH,
        GPIO_PIN_HIGH
    };
    const uint32_t waveform_len =
        (uint32_t)(sizeof(waveform) / sizeof(waveform[0]));

    for (uint32_t i = 0U; i < waveform_len; i++)
    {
        GPIO_EdgeCounter_Sample(waveform[i]);
    }

    printf("\n  Total edges detected: %u\n", GPIO_EdgeCounter_GetCount());
    printf("\n  Observation:\n");
    printf("    Module state (prev pin level, edge count) is held in static\n");
    printf("    file-scope variables - zero globals, full encapsulation.\n");

    /* -----------------------------------------------------------------------
     * SECTION 4 - Finite State Machine (FSM)
     * --------------------------------------------------------------------- */
    Console_PrintSeparator("4. Embedded Use-Case: Finite State Machine (FSM)");

    printf("\n  Simulating %u scheduler ticks:\n\n", DEMO_ITERATIONS);
    for (uint32_t i = 0U; i < DEMO_ITERATIONS; i++)
    {
        FSM_Tick();
    }

    printf("\n  Observation:\n");
    printf("    s_state persists between ticks - FSM transitions correctly\n");
    printf("    without any global variable or external state storage.\n");

    /* -----------------------------------------------------------------------
     * SECTION 5 - One-time initialization guard
     * --------------------------------------------------------------------- */
    Console_PrintSeparator("5. Embedded Use-Case: One-Time Initialization Guard");

    printf("\n  Calling Peripheral_Init() %u times:\n\n", DEMO_ITERATIONS);
    for (uint32_t i = 0U; i < DEMO_ITERATIONS; i++)
    {
        Peripheral_Init();
    }

    printf("\n  Peripheral_IsInitialized() = %s\n",
           Peripheral_IsInitialized() ? "true" : "false");
    printf("\n  Observation:\n");
    printf("    Hardware init runs ONCE; every subsequent call is a no-op.\n");
    printf("    Safe to call from multiple boot paths with no side effects.\n");

    /* -----------------------------------------------------------------------
     * Summary
     * --------------------------------------------------------------------- */
    Console_PrintSeparator("Summary - Local vs Static Properties");
    Console_PrintSummaryTable();

    return EXIT_SUCCESS;
}

/* End of file — main.c */
