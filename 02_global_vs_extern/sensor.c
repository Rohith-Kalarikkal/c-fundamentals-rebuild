/**
 * @file    sensor.c
 * @brief   Sensor module implementation.
 *
 * DESIGN PATTERN - Module-private state with 'static' globals:
 *   Variables declared 'static' at file scope have the same lifetime as
 *   true globals (they persist for the entire program) but the linker 
 *   cannot see them from any other translation unit.
 *
 *   This is the preferred embedded pattern:
 *     ->  Encapsulates state (no other module can corrupt it accidentally).
 *     ->  Avoids polluting the global namespace.
 *     ->  State is still exposed safely via accessor functions (e.g.
 *         Sensor_GetReadCount), maintaining a clean API boundary.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdlib.h>   /* rand() - simulates ADC reading on hosted build */

#include "sensor.h"
#include "system_config.h"   /* extern globals: g_system_initialised, g_error_count */

/* =========================================================================
 * MODULE-PRIVATE (FILE-SCOPE) STATIC GLOBALS
 *
 * 'static' at file scope means:
 *   - Storage is allocated once (lives for entire program).
 *   - Linkage is INTERNAL — invisible outside this translation unit.
 *   - Trying to 'extern' these from another file will cause a linker error.
 * ========================================================================= */

/** Whether this module has been initialised. */
static bool s_sensor_initialised = false;

/** Running count of successful sensor reads. */
static uint32_t s_read_count = 0U;

/**
 * Last raw ADC value read from hardware.
 * Callers get it only through Sensor_Read(), never directly.
 */
static uint32_t s_last_raw_value = 0U;

/**
 * PRIVATE HELPER (FORWARD DECLARATION)
 */
static uint32_t prvSimulateAdcRead(void);

/* =========================================================================
 * PUBLIC API IMPLEMENTATION
 * ========================================================================= */

SensorStatus_t Sensor_Init(void)
{
    /* Guard: system must be initialised before any module. */
    if (!g_system_initialised)   /* 'extern' g_system_initialised used here */
    {
        printf("[SENSOR] ERROR: System not initialised. Aborting sensor init.\n");
        INCREMENT_ERROR_COUNT();  /* macro uses extern g_error_count */
        return SENSOR_ERROR;
    }

    s_sensor_initialised = true;
    s_read_count         = 0U;
    s_last_raw_value     = 0U;

    printf("[SENSOR] Initialised successfully.\n");
    return SENSOR_OK;
}

SensorStatus_t Sensor_Read(uint32_t * const p_value)
{
    if (p_value == NULL)
    {
        printf("[SENSOR] ERROR: NULL pointer passed to Sensor_Read().\n");
        INCREMENT_ERROR_COUNT();
        return SENSOR_ERROR;
    }

    if (!s_sensor_initialised)
    {
        printf("[SENSOR] ERROR: Module not initialised.\n");
        INCREMENT_ERROR_COUNT();
        return SENSOR_ERROR;
    }

    /* Simulate a hardware ADC read. */
    s_last_raw_value = prvSimulateAdcRead();
    s_read_count++;

    *p_value = s_last_raw_value;

    printf("[SENSOR] Read #%u -> raw value = %u  (tick = %u ms)\n",
           s_read_count, s_last_raw_value,
           (unsigned int)g_system_tick_ms);  /* extern volatile read */

    return SENSOR_OK;
}

uint32_t Sensor_GetReadCount(void)
{
    /*
     * Accessor function for the private static counter.
     * The caller gets the data it needs without direct access to the variable.
     */
    return s_read_count;
}

/**
 * @brief  Simulate an ADC reading (replaces real hardware HAL call).
 * @return Simulated 12-bit ADC value (0–4095).
 */
static uint32_t prvSimulateAdcRead(void)
{
    return (uint32_t)(rand() % 4096);
}
