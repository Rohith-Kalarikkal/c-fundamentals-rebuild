/**
 * @file    actuator.c
 * @brief   Actuator module implementation.
 *
 * Mirrors the same static-global pattern used in sensor.c.
 * Also demonstrates reading the shared extern global 'g_system_tick_ms'
 * from yet another translation unit confirming there is only ONE copy
 * of that variable in the final binary (allocated in system_config.c).
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include "actuator.h"
#include "system_config.h"

/* =========================================================================
 * MODULE-PRIVATE STATIC GLOBALS
 * ========================================================================= */

/** Initialisation state of this module. */
static bool s_actuator_initialised = false;

/**
 * Current drive level (0-100 %).
 * Private: only Actuator_SetLevel() and Actuator_IsActive() access this.
 */
static uint8_t s_current_level = 0U;

/** Timestamp (tick) of the last level change, for diagnostics. */
static uint32_t s_last_change_tick = 0U;

/* =========================================================================
 * PUBLIC API IMPLEMENTATION
 * ========================================================================= */

ActuatorStatus_t Actuator_Init(void)
{
    if (!g_system_initialised)  /* shared extern - same variable as in sensor.c */
    {
        printf("[ACTUATOR] ERROR: System not initialised.\n");
        INCREMENT_ERROR_COUNT();
        return ACTUATOR_ERROR;
    }

    s_actuator_initialised = true;
    s_current_level        = 0U;
    s_last_change_tick     = g_system_tick_ms;  /* read shared extern tick */

    printf("[ACTUATOR] Initialised successfully.\n");
    return ACTUATOR_OK;
}

ActuatorStatus_t Actuator_SetLevel(uint8_t level)
{
    if (!s_actuator_initialised)
    {
        printf("[ACTUATOR] ERROR: Module not initialised.\n");
        INCREMENT_ERROR_COUNT();
        return ACTUATOR_ERROR;
    }

    if (level > 100U)
    {
        printf("[ACTUATOR] ERROR: Level %u exceeds maximum (100).\n", level);
        INCREMENT_ERROR_COUNT();
        return ACTUATOR_ERROR;
    }

    s_current_level    = level;
    s_last_change_tick = g_system_tick_ms;   /* extern volatile read */

    printf("[ACTUATOR] Level set to %u%%  (tick = %u ms)\n",
           s_current_level, (unsigned int)s_last_change_tick);

    return ACTUATOR_OK;
}

bool Actuator_IsActive(void)
{
    return (s_current_level > 0U);
}
