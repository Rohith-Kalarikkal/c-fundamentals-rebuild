/**
 * @file    fsm.h
 *
 * @brief   Public API for the demonstration Finite State Machine module.
 *
 * @details The FSM state register is a static local inside fsm.c — it is
 *          completely invisible to the rest of the application yet persists
 *          for the program's lifetime, exactly as required for an FSM.
 *
 *          Caller contract:
 *            - Call FSM_Tick() from the periodic scheduler (e.g., every 10 ms).
 *            - Optionally call FSM_GetState() to read current state for logging.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef FSM_H
#define FSM_H

#include "app_types.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief   Execute one FSM tick — evaluate transitions and run entry actions.
 *
 * @details Must be called from a periodic context (scheduler tick, RTOS task,
 *          or SysTick ISR). The static-local state register ensures the FSM
 *          remembers its current state between consecutive ticks.
 */
void FSM_Tick(void);

/**
 * @brief   Return the current FSM state 
 *
 * @return  FsmState_t  Current state of the machine.
 */
FsmState_t FSM_GetState(void);

#endif /* FSM_H */

/* End of file — fsm.h */
