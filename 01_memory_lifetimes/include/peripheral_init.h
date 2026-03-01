/**
 * @file    peripheral_init.h
 *
 * @brief   Public API for the peripheral one-time initialization module.
 *
 * @details The initialization guard is implemented with a static local bool
 *          inside peripheral_init.c. This ensures the hardware setup sequence
 *          runs exactly once regardless of how many call sites invoke the
 *          function — a clean, thread-unsafe singleton init pattern.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef PERIPHERAL_INIT_H
#define PERIPHERAL_INIT_H

#include <stdbool.h>

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief   Initialize the peripheral — executes only on the very first call.
 *
 * @details Subsequent calls are silently skipped via an internal static-local
 *          guard flag. Safe to call from multiple code paths.
 */
void Peripheral_Init(void);

/**
 * @brief   Query whether the peripheral has been initialized.
 *
 * @return  true   Peripheral has been successfully initialized.
 * @return  false  Peripheral_Init() has not yet been called (or init failed).
 */
bool Peripheral_IsInitialized(void);

#endif /* PERIPHERAL_INIT_H */

/* End of file — peripheral_init.h */
