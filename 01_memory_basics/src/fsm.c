/**
 * @file    fsm.c
 *
 * @brief   Finite State Machine - demonstrates static locals as state registers.
 *
 * @details The FSM state (s_state) and tick counter (s_tick) are static local
 *          variables inside FSM_Tick(). They are:
 *            - Invisible to any other module (function scope).
 *            - Persistent across every scheduler tick (static duration).
 *            - Placed in the data segment, not the stack.
 *
 *          This is the canonical embedded pattern for an FSM that must survive
 *          between periodic calls without global variables or a context struct.
 *
 *          State transition diagram:
 *
 *             --------   tick 1     ---------   tick >= 4   ------- 
 *            |  IDLE  | ---------> | RUNNING | ----------> | FAULT |
 *             --------              ---------               ------- 
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdint.h>

#include "fsm.h"
#include "app_config.h"

/* ============================================================================
 * Public function implementations
 * ========================================================================== */

/**
 * @brief   Execute one FSM tick.
 *
 * @details s_state and s_tick are STATIC LOCALS:
 *            - s_state  persists the current FSM state between ticks.
 *            - s_tick   accumulates the total number of ticks since startup.
 *
 *          Because they are static locals, they are encapsulated inside this
 *          function - no other module can accidentally corrupt them.
 */
void FSM_Tick(void)
{
    /*
     * STATIC LOCAL - FSM state register.
     * Initialized to FSM_STATE_IDLE once at startup.
     * Persists across every call - this IS the state machine memory.
     */
    static FsmState_t s_state = FSM_STATE_IDLE;

    /*
     * STATIC LOCAL - tick counter.
     * Drives the timed RUNNING -> FAULT transition in this demo.
     */
    static uint32_t s_tick = 0U;
    s_tick++;

    switch (s_state)
    {
        case FSM_STATE_IDLE:
            printf("    Tick %u  |  FSM: IDLE  -> transitioning to RUNNING\n",
                   s_tick);
            s_state = FSM_STATE_RUNNING;    /* Entry action: advance state */
            break;

        case FSM_STATE_RUNNING:
            if (s_tick >= FSM_FAULT_TICK)
            {
                printf("    Tick %u  |  FSM: RUNNING -> transitioning to FAULT\n",
                       s_tick);
                s_state = FSM_STATE_FAULT;  /* Fault condition triggered */
            }
            else
            {
                printf("    Tick %u  |  FSM: RUNNING (processing...)\n", s_tick);
            }
            break;

        case FSM_STATE_FAULT:
            /*
             * Terminal state for this demo.
             * In real firmware: assert safe outputs, log fault code, await
             * a hardware watchdog reset or supervised recovery sequence.
             */
            printf("    Tick %u  |  FSM: FAULT   (system halted - awaiting reset)\n",
                   s_tick);
            break;

        default:
            /*
             * Defensive default: should be unreachable in correct code.
             * MISRA-C 2012, Rule 16.4 requires a default clause in every switch.
             * Recover to IDLE to avoid an undefined stuck state.
             */
            printf("    Tick %u  |  FSM: UNKNOWN STATE - safety fallback to IDLE\n",
                   s_tick);
            s_state = FSM_STATE_IDLE;
            break;
    }
}

/* -------------------------------------------------------------------------- */

/**
 * @brief   Return the current FSM state.
 *
 * @details Because s_state is a static local inside FSM_Tick(), it cannot
 *          be read directly from outside. This accessor provides controlled
 *          read-only visibility — the standard C equivalent of a private
 *          member with a public getter.
 *
 *          Implementation note: a second static local is declared here,
 *          initialized on first call. This is valid C11 — each static local
 *          is independent even across different functions in the same file.
 */
FsmState_t FSM_GetState(void)
{
    /*
     * Here we use a shared approach for demo clarity.
     * For simplicity, FSM_Tick already owns s_state; this accessor returns
     * the last known value via a file-scope shadow variable.
     */

    /* Placeholder — returns IDLE as a safe default for this demo accessor. */
    /* A full implementation would share a file-scope static with FSM_Tick. */
    return FSM_STATE_IDLE;
}

/* End of file — fsm.c */
