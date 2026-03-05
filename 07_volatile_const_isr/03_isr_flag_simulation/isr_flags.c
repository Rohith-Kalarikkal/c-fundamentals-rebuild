/**
 * @file    isr_flags.c
 * @brief   Volatile ISR flag definitions and simulated ISR bodies.
 *
 * DESIGN PATTERN - ISR <-> Main-loop deferred processing
 * ------------------------------------------------------------------------
 *
 *    --------------   sets volatile flag     --------------------------
 *   │  Hardware    │ ---------------------> │  ISR (short, fast)       │
 *   │  Peripheral  │                        │  g_uart_rx_flag = true;  │
 *    --------------                         │  g_uart_rx_byte = DR;    │
 *                                            --------------------------
 *                                                       │ flag visible to
 *                                                       │ main after ISR ret
 *                                             --------------------------
 *                                            │  Main loop               │
 *                                            │  if (g_uart_rx_flag) {   │
 *                                            │    handle_uart_rx();     │
 *                                            │  }                       │
 *                                             --------------------------
 *
 * VOLATILE NECESSITY HERE:
 *   The ISR runs in a completely separate execution context (it can preempt
 *   main at any instruction boundary). The C compiler, however, analyses
 *   functions independently. It will see the main loop reads g_uart_rx_flag
 *   in every iteration but no C-level code in the loop modifies it, so it
 *   is free to cache the value in a CPU register. volatile forces a fresh
 *   load from memory on each loop iteration.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "isr_flags.h"
#include <stdio.h>

/* ------------------------------------------------------------------------
 * Volatile global flag definitions
 *
 * Each must be initialised to a known state at program start.
 * ------------------------------------------------------------------------ */
volatile bool     g_uart_rx_flag        = false;
volatile uint8_t  g_uart_rx_byte        = 0U;
volatile bool     g_timer_tick_flag     = false;
volatile uint32_t g_tick_ms             = 0UL;
volatile bool     g_button_pressed_flag = false;

/* ------------------------------------------------------------------------
 * Simulated ISR bodies
 *
 * On a real MCU:
 *   void USART1_IRQHandler(void) { ... }   <- placed in .isr_vector section
 *
 * Rules inside a real ISR:
 *   • MUST be fast (microseconds, not milliseconds)
 *   • NO dynamic memory allocation, NO printf, NO blocking calls
 *   • Only set flags, copy data, clear interrupt pending bit
 * ========================================================================= */

void sim_uart_rx_isr(uint8_t byte)
{
    /*
     * Do the absolute minimum:
     *   1. Read the data register (clears RXNE on real HW).
     *   2. Copy to shared variable.
     *   3. Set flag.
     *   4. Return — CPU restores context.
     */
    g_uart_rx_byte  = byte;          /* Store byte BEFORE setting flag */
    MEMORY_BARRIER();                /* Prevent compiler reordering     */
    g_uart_rx_flag  = true;          /* Announce to main loop           */

    printf("  [ISR/UART_RX] Byte 0x%02X captured. Flag set.\n", (unsigned)byte);
}

void sim_timer_isr(void)
{
    g_tick_ms++;                     /* Atomic on 32-bit aligned uint32 */
    g_timer_tick_flag = true;
    /* NOTE: no printf here - too slow for a 1 ms SysTick ISR in production */
}

void sim_button_isr(void)
{
    /*
     * Debouncing should normally be done in hardware (RC filter) or via a
     * software timer started here. For the program we simply set the flag.
     */
    g_button_pressed_flag = true;
    printf("  [ISR/BUTTON] Button press detected. Flag set.\n");
}

/* ------------------------------------------------------------------------
 * Main-loop handler implementations
 * ------------------------------------------------------------------------ */

void handle_uart_rx(void)
{
    /*
     * CRITICAL: clear the flag AFTER reading the data, never before.
     * Clearing first creates a race: ISR could fire between clear and read,
     * and main would miss the new byte.
     */
    uint8_t byte = g_uart_rx_byte;   /* Read data first   */
    MEMORY_BARRIER();
    g_uart_rx_flag = false;          /* Then clear flag   */

    printf("  [MAIN] handle_uart_rx: Processing byte 0x%02X ('%c')\n",
           (unsigned)byte,
           (byte >= 0x20U && byte <= 0x7EU) ? (char)byte : '.');
}

void handle_timer_tick(void)
{
    uint32_t current_tick = g_tick_ms;
    g_timer_tick_flag = false;

    /* Typical 1 ms tasks: software timer decrement, LED blink counter, etc. */
    printf("  [MAIN] handle_timer_tick: tick=%lu\n", (unsigned long)current_tick);
}

void handle_button_press(void)
{
    g_button_pressed_flag = false;
    printf("  [MAIN] handle_button_press: Toggling LED state.\n");
}
