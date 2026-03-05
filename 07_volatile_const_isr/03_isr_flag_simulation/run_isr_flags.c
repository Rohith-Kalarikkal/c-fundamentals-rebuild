/**
 * @file    run_isr_flags.c
 * @brief   Simulates an interrupt-driven bare-metal main loop.
 *
 * We manually fire ISRs in a controlled sequence and show the main loop
 * responding to each volatile flag - exactly as it would on an MCU.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "isr_flags.h"
#include <stdio.h>

/**
 * @brief  One iteration of the bare-metal super-loop ("background task").
 *         On a real MCU this runs continuously. The CPU spends most of its
 *         time here and is only interrupted briefly by ISRs.
 */
static void main_loop_iteration(void)
{
    /*
     * Check each volatile flag in priority order.
     * Flags are polled, not nested - keeps logic simple on small MCUs.
     */
    if (g_uart_rx_flag)
    {
        handle_uart_rx();
    }

    if (g_timer_tick_flag)
    {
        handle_timer_tick();
    }

    if (g_button_pressed_flag)
    {
        handle_button_press();
    }
}

void run_isr_flags(void)
{
    printf("\n------------------------------------------------------------------------\n");
    printf("               ISR Flag Simulation (volatile flag pattern)                \n");
    printf("------------------------------------------------------------------------\n\n");

    printf("Scenario: main loop idle, then 3 \"interrupts\" fire\n\n");

    /* --- Iteration 1: no interrupts pending -------------------------------- */
    printf("[LOOP iter 1] No flags set - main loop finds nothing to do.\n");
    main_loop_iteration();
    printf("  (No handlers called - correct idle behaviour)\n\n");

    /* --- Fire UART RX ISR ----------------------------------------------------- */
    printf("[EVENT] UART hardware places byte 0x48 ('H') in DR - fires ISR\n");
    sim_uart_rx_isr((uint8_t)'H');

    /* --- Iteration 2: UART RX flag set ------------------------------------- */
    printf("\n[LOOP iter 2] Checking flags...\n");
    main_loop_iteration();
    printf("\n");

    /* --- Fire timer ISR three times --------------------------------------- */
    printf("[EVENT] Timer ISR fires (1 ms elapsed)\n");
    sim_timer_isr();
    printf("[EVENT] Timer ISR fires (1 ms elapsed)\n");
    sim_timer_isr();

    printf("\n[LOOP iter 3] Checking flags (timer_tick only)...\n");
    main_loop_iteration();   /* Clears tick flag; only one handler fires per flag set */
    printf("\n");

    /* --- Fire button ISR ----------------------------------------------- */
    printf("[EVENT] Button GPIO EXTI — fires ISR\n");
    sim_button_isr();

    /* --- Fire UART RX again ------------------------------------------- */
    printf("[EVENT] Another UART byte 0x57 ('W') arrives\n");
    sim_uart_rx_isr((uint8_t)'W');

    /* --- Iteration 4: multiple flags pending ---------------------------- */
    printf("\n[LOOP iter 4] Multiple flags pending — handle in priority order:\n");
    main_loop_iteration();
    printf("\n");

    /* --- Summary ------------------------------------------------------ */
    printf("--- ISR flag lesson summary -------------------------------\n");
    printf(
        "  volatile bool flag pattern guarantees:\n"
        "    - Main loop always sees ISR-written flag (no stale cache)\n"
        "    - ISR stays short - no blocking, no heap, no printf\n"
        "    - MEMORY_BARRIER prevents reordering of flag vs data writes\n"
        "    - Clear flag AFTER reading data (avoids race condition)\n"
        "  When to upgrade beyond simple volatile:\n"
        "    - Multiple writers -> use atomic_flag or critical section\n"
        "    - Queue of events  -> use ring buffer protected by critical sec\n"
        "    - RTOS environment -> use semaphore / event group\n"
    );
}
