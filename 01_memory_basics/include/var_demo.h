/**
 * @file    var_demo.h
 *
 * @brief   Public API for the local vs static variable core demonstrations.
 *
 * @details Exposes only what callers need — the two run functions.
 *          All internal implementation details remain private in var_demo.c.
 *
 *          Include guard follows the pattern:  <FILENAME_CAPS>_H
 *          Consistent naming across the project avoids guard collisions.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef VAR_DEMO_H
#define VAR_DEMO_H

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief   Run the local variable demonstration.
 *
 * Allocates a counter on the stack, increments it, and prints its value
 * and address. Demonstrates that value and (in practice) address reset
 * on every invocation.
 */
void VarDemo_RunLocal(void);

/**
 * @brief   Run the static local variable demonstration.
 *
 * Increments a persistent counter and prints its value and address.
 * Demonstrates that value accumulates and address is fixed across calls.
 */
void VarDemo_RunStatic(void);

/**
 * @brief   Run the memory address comparison demonstration.
 *
 * Prints the addresses of a local and a static local side-by-side so the
 * caller can observe stack vs data-segment placement across two calls.
 */
void VarDemo_RunAddressComparison(void);

#endif /* VAR_DEMO_H */

/* End of file — var_demo.h */
