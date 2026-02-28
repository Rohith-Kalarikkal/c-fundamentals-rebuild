/**
 * @file    peripheral_init.c
 *
 * @brief   Peripheral initialization — demonstrates a static local init guard.
 *
 * @details The classic embedded "init once" pattern:
 *
 *          A static local bool (s_initialized) acts as a latch. On the very
 *          first call it is false -> the init sequence runs -> it is set to true.
 *          Every subsequent call finds it true and immediately returns, making
 *          the function safe to call from multiple code paths (e.g., from both
 *          the boot sequence and a late-init recovery path) without repeating
 *          expensive or state-corrupting hardware setup.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdbool.h>

#include "peripheral_init.h"

/* ============================================================================
 * Module-private state
 * ========================================================================== */

/**
 * Initialization latch - file-scope static so Peripheral_IsInitialized()
 * can read it without re-running any init logic.
 *
 * false -> not yet initialized (default BSS value).
 * true  -> initialization has completed successfully.
 */
static bool s_initialized = false;

/* ============================================================================
 * Public function implementations
 * ========================================================================== */

/**
 * @brief   Initialize the peripheral - executes hardware setup exactly once.
 *
 * @details The guard check is the very first operation. Any code path that
 *          calls this function after the first successful init incurs only
 *          the cost of a single boolean comparison and a branch — negligible
 *          overhead even in a tight ISR.
 */
void Peripheral_Init(void)
{
    if (s_initialized)
    {
        /*
         * Already initialized — skip silently.
         * Print only for demo visibility; production code would return here
         * with no side effects.
         */
        printf("    [SKIP] Peripheral already initialized - skipping init.\n");
        return;
    }

    /*
     * --- First-call path: run the hardware initialization sequence. ---
     */
    printf("    [INIT] Peripheral clock enabled.\n");
    printf("    [INIT] DMA channels configured.\n");
    printf("    [INIT] Interrupt priority set - peripheral READY.\n");

    s_initialized = true;   /* Latch: all future calls will take the skip path */
}

/* -------------------------------------------------------------------------- */

/**
 * @brief   Query whether the peripheral has been initialized.
 */
bool Peripheral_IsInitialized(void)
{
    return s_initialized;
}

/* End of file — peripheral_init.c */
