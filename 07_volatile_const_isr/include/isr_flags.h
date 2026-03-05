/**
 * @file    isr_flags.h
 * @brief   ISR ↔ main-loop communication via volatile flags.
 *
 * In a real RTOS or bare-metal system an ISR cannot safely call long-running
 * functions (memory allocation, printf, etc.). The canonical pattern is:
 *
 *   ISR:        sets a volatile flag.
 *   Main loop:  polls the flag, clears it, then performs the heavy work.
 *
 * WHY volatile FOR ISR FLAGS?
 * The compiler analyses only one "thread" at a time. In the main loop it
 * sees:
 *
 *       while (!g_uart_rx_flag) { }  // wait
 *
 * From the compiler's view nothing inside the loop modifies g_uart_rx_flag,
 * so it MAY transform this to:
 *
 *       if (!g_uart_rx_flag) { while(1){} }  // infinite loop!
 *
 * Declaring the flag volatile tells the compiler: "this variable can change
 * at ANY time by code outside your analysis — always re-read from memory."
 *
 * WHY NOT just use an atomic or mutex?
 *   On small Cortex-M0 cores without OS support, a simple volatile bool is
 *   sufficient when:
 *     a) the flag is written ONLY by the ISR (single writer), and
 *     b) the flag is a naturally aligned type (single bus transaction = atomic).
 *   For multi-writer scenarios, use atomic_flag or a critical-section guard.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef ISR_FLAGS_H
#define ISR_FLAGS_H

#include "platform_types.h"

/* ------------------------------------------------------------------
 * Shared volatile flags — written by ISR, read by main loop
 *
 * Rule: ONLY the ISR may SET these.
 *       ONLY the main loop may CLEAR them (after servicing).
 * ------------------------------------------------------------------ */
extern volatile uint32_t isr_flags;
void run_isr_flags(void);

/** Set by UART RX ISR when a full byte has been received. */
extern volatile bool g_uart_rx_flag;

/** The byte captured by the UART RX ISR (valid when g_uart_rx_flag == true). */
extern volatile uint8_t g_uart_rx_byte;

/** Set by Timer ISR every 1 ms — used for software timers in main loop. */
extern volatile bool g_timer_tick_flag;

/** Counts milliseconds since startup — incremented only inside Timer ISR. */
extern volatile uint32_t g_tick_ms;

/** Set by GPIO EXTI ISR when a button press is detected. */
extern volatile bool g_button_pressed_flag;

/* --------------------------------------------------------------------------
 * Simulated ISR functions
 *
 * On a real MCU these would be placed in the vector table and triggered by
 * hardware. Here we call them explicitly from main() to demonstrate the
 * volatile flag contract without needing actual interrupts.
 * -------------------------------------------------------------------------- */

/**
 * @brief  Simulates UART RX interrupt: receives one byte and sets the flag.
 * @param  byte  The byte the "hardware" placed in the data register.
 */
void sim_uart_rx_isr(uint8_t byte);

/**
 * @brief  Simulates SysTick / Timer interrupt: increments tick counter.
 *         Called once per simulated millisecond.
 */
void sim_timer_isr(void);

/**
 * @brief  Simulates GPIO EXTI interrupt: a button was pressed.
 */
void sim_button_isr(void);

/* --------------------------------------------------------------------------
 * Main-loop handler prototypes
 * -------------------------------------------------------------------------- */

/**
 * @brief  Called from main loop when g_uart_rx_flag is set.
 *         Clears the flag and processes the received byte.
 */
void handle_uart_rx(void);

/**
 * @brief  Called from main loop when g_timer_tick_flag is set.
 *         Clears the flag and runs 1 ms software tasks.
 */
void handle_timer_tick(void);

/**
 * @brief  Called from main loop when g_button_pressed_flag is set.
 *         Clears the flag and handles button-press logic.
 */
void handle_button_press(void);

#endif /* ISR_FLAGS_H */
