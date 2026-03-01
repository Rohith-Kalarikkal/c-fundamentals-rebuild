/**
 * @file    var_demo.c
 *
 * @brief   Core demonstration: local (auto) vs static local variable behavior.
 *
 * @details Three demonstrations are provided:
 *
 *          1. VarDemo_RunLocal()
 *             Shows that a local variable is stack-allocated and re-initialized
 *             on every function call. Value never persists — always resets to 0.
 *
 *          2. VarDemo_RunStatic()
 *             Shows that a static local lives in the data segment and retains
 *             its value across all calls. Address is fixed for program lifetime.
 *
 *          3. VarDemo_RunAddressComparison()
 *             Prints both addresses side-by-side so the caller can observe
 *             the stack address fluctuating while the static address stays fixed.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdint.h>

#include "var_demo.h"

/* ============================================================================
 * Public function implementations
 * ========================================================================== */

/**
 * @brief   Demonstrates a LOCAL (auto) variable - re-created on every call.
 *
 * @details 'counter' is allocated on the current stack frame when the function
 *          is entered and deallocated when it returns. Explicitly initializing
 *          to 0 on declaration (MISRA-C 2012, Rule 9.1) prevents undefined
 *          behavior. The increment always produces 1 because the starting
 *          value is always 0 — the previous value is gone.
 *
 *          Memory note: on most architectures the stack pointer is merely
 *          adjusted; the physical memory is not zeroed by the CPU. Without
 *          explicit initialization, reading the variable yields garbage.
 */
void VarDemo_RunLocal(void)
{
    /*
     * LOCAL variable — stack-allocated.
     * Lifetime  : this function call only.
     * Segment   : stack (SP-relative address).
     */
    uint32_t counter = 0U;

    counter++;  /* Increments from 0 -> always yields 1 on every call */

    printf("    Local  variable  |  counter = %2u  |  &counter = %p\n",
           counter, (void *)&counter);
}

/* -------------------------------------------------------------------------- */

/**
 * @brief   Demonstrates a STATIC LOCAL variable — persists across all calls.
 *
 * @details 'counter' is placed in the BSS (zero-init) or Data segment by the
 *          linker. The C runtime initializes it to 0 before main() runs.
 *          Every subsequent call finds it exactly as the previous call left it.
 *
 *          The address printed will be IDENTICAL on every call, confirming
 *          that this is a single fixed location - not a stack frame slot.
 *
 *          Encapsulation note: 'counter' is invisible outside this function
 *          (function scope) even though it lives at static storage duration.
 *          This is the key advantage over a file-scope or global variable.
 */
void VarDemo_RunStatic(void)
{
    /*
     * STATIC LOCAL variable - data/BSS segment.
     * Lifetime  : entire program execution.
     * Segment   : .bss (zero-init) or .data (non-zero init).
     * Scope     : this function only (compiler-enforced encapsulation).
     */
    static uint32_t counter = 0U;

    counter++;  /* Accumulates across calls: 1, 2, 3, 4 … */

    printf("    Static variable  |  counter = %2u  |  &counter = %p\n",
           counter, (void *)&counter);
}

/* -------------------------------------------------------------------------- */

/**
 * @brief   Side-by-side address comparison: stack (local) vs data (static).
 *
 * @details Key safety implication demonstrated here:
 *            - Returning &local_var from a function → UNDEFINED BEHAVIOR
 *              (dangling pointer; stack frame no longer valid after return).
 *            - Returning &static_var from a function → safe (fixed address),
 *              but the caller must handle non-reentrancy carefully.
 */
void VarDemo_RunAddressComparison(void)
{
    uint32_t        local_var  = 42U;   /* Stack   — address is call-relative  */
    static uint32_t static_var = 42U;  /* Data    — address is link-time fixed */

    printf("    Local  variable  address: %p   (stack - changes each call)\n",
           (void *)&local_var);
    printf("    Static variable  address: %p   (data segment - always fixed)\n",
           (void *)&static_var);
}

/* End of file — var_demo.c */
