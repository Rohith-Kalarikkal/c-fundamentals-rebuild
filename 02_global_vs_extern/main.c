/**
 * @file    main.c
 * @brief   Application entry point - demonstrates global vs extern in action.
 *
 * SUMMARY OF WHAT THIS PROGRAM PROVES
 * ─────────────────────────────────────────────────────────────────────────
 *  1. GLOBAL (definition)
 *       uint32_t g_system_tick_ms = 0U;   <- in system_config.c
 *       -> The compiler allocates ONE block of RAM for this variable.
 *
 *  2. EXTERN (declaration)
 *       extern volatile uint32_t g_system_tick_ms;  <- in system_config.h
 *       -> Included by main.c, sensor.c, actuator.c.
 *       -> All three translation units refer to the SAME RAM address.
 *       -> No extra storage is allocated; the linker resolves the symbol.
 *
 *  3. STATIC global (file scope)
 *       static uint32_t s_read_count;    <- inside sensor.c
 *       -> Allocated once (like a global) but invisible outside sensor.c.
 *       -> Accessing it from main.c is done safely via Sensor_GetReadCount().
 *
 *  4. Attempting to 'extern' a static from another file would cause:
 *       "undefined reference to `s_read_count`"  <- linker error
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdlib.h>   /* srand */
#include <time.h>     /* time  */

#include "system_config.h"   /* extern declarations for shared globals */
#include "sensor.h"
#include "actuator.h"

/* =========================================================================
 * LOCAL CONSTANTS
 * ========================================================================= */

#define SIMULATION_CYCLES       (5U)    /* Number of sense -> act loops to run. */
#define TICK_INCREMENT_PER_LOOP (10U)   /* Simulated ms per loop iteration.  */

/* =========================================================================
 * PRIVATE HELPER PROTOTYPES
 * ========================================================================= */
static void System_Startup(void);
static void System_Shutdown(void);
static void prvSimulateTick(uint32_t delta_ms);

/* =========================================================================
 * MAIN
 * ========================================================================= */

int main(void)
{
    uint32_t       sensor_value = 0U;
    uint8_t        drive_level  = 0U;
    SensorStatus_t sensor_status;
    ActuatorStatus_t actuator_status;

    srand((unsigned int)time(NULL));   /* seed RNG for simulated ADC */

    /* ------------------------------------------------------------------ */
    printf("\n========================================================\n");
    printf("  Global vs Extern - Embedded C Demonstration\n");
    printf("  System: %s\n", g_system_name);           /* extern const */
    printf("  FW:     v%u.%u\n",
           FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR);
    printf("========================================================\n\n");

    /* ------------------------------------------------------------------ */
    /* STEP 1: System startup — sets g_system_initialised = true          */
    System_Startup();

    /* ------------------------------------------------------------------ */
    /* STEP 2: Initialise modules                                          */
    /*         Each module reads g_system_initialised via extern.         */
    /* ------------------------------------------------------------------ */
    sensor_status   = Sensor_Init();
    actuator_status = Actuator_Init();

    if ((sensor_status != SENSOR_OK) || (actuator_status != ACTUATOR_OK))
    {
        printf("[MAIN] Module init failed. Halting.\n");
        return EXIT_FAILURE;
    }

    /* ------------------------------------------------------------------ */
    /* STEP 3: Main control loop                                           */
    /* ------------------------------------------------------------------ */
    printf("\n--- Control Loop (%u cycles) ---\n\n", SIMULATION_CYCLES);

    for (uint32_t cycle = 1U; cycle <= SIMULATION_CYCLES; cycle++)
    {
        printf("[MAIN] -- Cycle %u --\n", cycle);

        /* Simulate time passing (would be a real timer ISR on hardware). */
        prvSimulateTick(TICK_INCREMENT_PER_LOOP);

        /* Read sensor — stores result in sensor module's static variable. */
        sensor_status = Sensor_Read(&sensor_value);
        if (sensor_status != SENSOR_OK)
        {
            printf("[MAIN] Sensor read failed on cycle %u.\n", cycle);
            continue;
        }

        /*
         * Simple proportional control: map 12-bit ADC (0–4095) to
         * actuator drive level (0–100 %).
         */
        drive_level = (uint8_t)((sensor_value * 100UL) / 4095UL);
        actuator_status = Actuator_SetLevel(drive_level);

        if (actuator_status != ACTUATOR_OK)
        {
            printf("[MAIN] Actuator set failed on cycle %u.\n", cycle);
        }

        printf("\n");
    }

    /* ------------------------------------------------------------------ */
    /* STEP 4: End-of-run diagnostics using extern and module APIs        */
    /* ------------------------------------------------------------------ */
    printf("--- Diagnostics ---\n");
    printf("  Total sensor reads : %u\n", Sensor_GetReadCount());
    printf("  Actuator active    : %s\n",  Actuator_IsActive() ? "YES" : "NO");
    printf("  Final tick         : %u ms\n", (unsigned int)g_system_tick_ms);
    printf("  Total errors       : %u\n",   g_error_count);  /* extern */

    System_Shutdown();
    return EXIT_SUCCESS;
}

/* =========================================================================
 * PRIVATE HELPER IMPLEMENTATIONS
 * ========================================================================= */

/**
 * @brief  Perform system-level startup.
 *         Sets the shared global g_system_initialised to true so that
 *         all modules (reading via extern) know the system is ready.
 */
static void System_Startup(void)
{
    printf("[MAIN] System startup...\n");

    /*
     * Writing to a global defined in system_config.c.
     * main.c accesses it through the extern declaration in system_config.h.
     * sensor.c and actuator.c will see this updated value when they read it.
     */
    g_system_initialised = true;   /* ← ONE write, visible to ALL modules */

    printf("[MAIN] g_system_initialised set to TRUE "
           "(shared via extern across all modules).\n\n");
}

/**
 * @brief  Graceful shutdown.
 */
static void System_Shutdown(void)
{
    printf("\n[MAIN] Shutting down. Goodbye.\n");
    printf("========================================================\n\n");
    g_system_initialised = false;
}

/**
 * @brief  Increment the shared system tick counter.
 * @param  delta_ms  Milliseconds to add.
 *
 * On real hardware this would be called from a SysTick ISR.
 * Here it is called from the main loop to simulate time.
 */
static void prvSimulateTick(uint32_t delta_ms)
{
    /*
     * g_system_tick_ms is declared 'volatile' because in production it is
     * modified by an ISR. The volatile qualifier prevents the compiler from
     * caching it in a register between reads in the main loop.
     */
    g_system_tick_ms += delta_ms;   /* extern volatile — direct write */
}
