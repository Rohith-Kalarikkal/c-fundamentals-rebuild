/**
 * @file    volatile_registers.c
 * @brief   Demonstrates WHY volatile is mandatory for hardware registers.
 *
 * FLOW
 * -----
 *  1. Transmit a byte: poll TXE (busy-wait), write DR, poll TC (wait done).
 *  2. Receive a byte:  poll RXNE, read DR.
 *  3. Enable RXNE interrupt: set RXNEIE in CR1.
 *  4. Show what WRONG (non-volatile) polling looks like in assembly terms.
 *
 * On every poll we call hw_uart_simulate_tick() to advance the fake
 * peripheral state machine - this replaces the real hardware clock.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "hw_registers.h"
#include <stdio.h>

/* --------------------------------------------------------------------------
 * Private helpers
 * -------------------------------------------------------------------------- */

/**
 * @brief  Blocking transmit - polls TXE then writes one byte to DR.
 *         After writing we poll TC (transmission complete) with simulated ticks.
 *
 * VOLATILE CONTRACT:
 *   Each loop iteration MUST re-read SR from memory (not a cached register).
 *   Without volatile, gcc -O2 would hoist the SR read before the while(),
 *   turning the poll into "if TXE not set → hang forever".
 */
static status_t uart_transmit_byte(UART_RegMap_t *uart, uint8_t byte)
{
    uint32_t timeout = 10000U;

    /* ---- Step 1: wait until transmit buffer is empty (TXE=1) ----------- */
    printf("  [TX] Waiting for TXE (transmit buffer empty)...\n");
    while (!TST_BIT(uart->SR, UART_SR_TXE))
    {
        hw_uart_simulate_tick(uart);
        if (--timeout == 0U) { return STATUS_TIMEOUT; }
    }

    /* ---- Step 2: load byte into data register -------------------------- */
    /*
     * Writing DR triggers hardware to start shifting out the byte.
     * Hardware will clear TXE and eventually set TC.
     * VOLATILE ensures this write is not eliminated even if DR is
     * "never read back" in this function.
     */
    uart->DR = byte;
    CLR_BIT(uart->SR, UART_SR_TXE);   /* Simulate HW clearing TXE on DR write */
    printf("  [TX] Wrote 0x%02X ('%c') to DR.\n", byte,
           (byte >= 0x20U && byte <= 0x7EU) ? (char)byte : '.');

    /* ---- Step 3: wait for transmission complete (TC=1) ----------------- */
    timeout = 10000U;
    printf("  [TX] Waiting for TC (transmission complete)...\n");
    while (!TST_BIT(uart->SR, UART_SR_TC))
    {
        hw_uart_simulate_tick(uart);
        if (--timeout == 0U) { return STATUS_TIMEOUT; }
    }

    printf("  [TX] Byte transmitted successfully.\n");
    return STATUS_OK;
}

/**
 * @brief  Simulate an incoming byte from the "wire" and demonstrate RX poll.
 *
 * On real hardware, the UART peripheral would:
 *   1. Detect the start bit.
 *   2. Shift 8 bits into its receive shift register.
 *   3. Copy the byte to DR and set RXNE.
 * We replicate step 3 manually.
 */
static void simulate_rx_byte(UART_RegMap_t *uart, uint8_t incoming)
{
    printf("  [SIM] Remote device sends byte 0x%02X ('%c')\n", incoming,
           (incoming >= 0x20U && incoming <= 0x7EU) ? (char)incoming : '.');
    uart->DR = incoming;
    SET_BIT(uart->SR, UART_SR_RXNE);
}

/**
 * @brief  Blocking receive - polls RXNE and reads DR.
 */
static status_t uart_receive_byte(UART_RegMap_t *uart, uint8_t *out_byte)
{
    uint32_t timeout = 10000U;

    printf("  [RX] Waiting for RXNE (receive buffer not empty)...\n");
    while (!TST_BIT(uart->SR, UART_SR_RXNE))
    {
        hw_uart_simulate_tick(uart);
        if (--timeout == 0U) { return STATUS_TIMEOUT; }
    }

    /*
     * VOLATILE CONTRACT:
     *   Reading DR both retrieves the byte AND (on real HW) clears RXNE.
     *   Without volatile the compiler might skip the read if it thinks
     *   the return value is unused, or it might read DR "early" before
     *   RXNE is actually set. volatile enforces exact ordering.
     */
    *out_byte = (uint8_t)uart->DR;
    CLR_BIT(uart->SR, UART_SR_RXNE);  /* Simulate HW clearing RXNE on DR read */

    printf("  [RX] Received 0x%02X ('%c')\n", *out_byte,
           (*out_byte >= 0x20U && *out_byte <= 0x7EU) ? (char)*out_byte : '.');
    return STATUS_OK;
}

/* --------------------------------------------------------------------------
 * volatile_registers  (called from main.c)
 * --------------------------------------------------------------------------*/
void volatile_registers(void)
{
    uint8_t   rx_byte = 0U;
    status_t  result;

    printf("\n--------------------------------------------------------------------------\n");
    printf("                   Volatile Hardware Register Simulation                   \n");
    printf("--------------------------------------------------------------------------\n\n");

    /* ------------------------------------------------------------------ */
    printf("--- Initialising peripherals ------------------------------------\n");
    hw_registers_init();
    hw_uart_dump(SIM_UART1, "After init");

    /* ------------------------------------------------------------------ */
    printf("--- Transmitting byte 'A' (0x41) --------------------------------\n");
    result = uart_transmit_byte(SIM_UART1, (uint8_t)'A');
    if (result == STATUS_OK)
    {
        hw_uart_dump(SIM_UART1, "After TX 'A'");
    }
    else
    {
        printf("  [ERROR] TX timed out!\n");
    }

    /* ------------------------------------------------------------------ */
    printf("--- Simulating incoming byte 'Z' (0x5A) -------------------------\n");
    simulate_rx_byte(SIM_UART1, (uint8_t)'Z');
    hw_uart_dump(SIM_UART1, "After remote sends 'Z'");

    result = uart_receive_byte(SIM_UART1, &rx_byte);
    if (result == STATUS_OK)
    {
        printf("  [APP] Application received: '%c'\n", rx_byte);
        hw_uart_dump(SIM_UART1, "After RX consumed");
    }

    /* ------------------------------------------------------------------ */
    printf("--- Enabling RXNE interrupt (interrupt-driven mode) -------------\n");
    /*
     * Set RXNEIE bit in CR1. Real hardware will now raise an IRQ on every
     * received byte instead of requiring software to poll RXNE.
     * volatile ensures this write is not reordered past other CR1 writes.
     */
    SET_BIT(SIM_UART1->CR1, UART_CR1_RXNEIE);
    printf("  RXNEIE set in CR1. CR1 = 0x%08X\n", (unsigned)SIM_UART1->CR1);

    /* ------------------------------------------------------------------ */
    printf("\n--- volatile lesson summary ----------------------------------\n");
    printf(
        "  Without volatile:\n"
        "    - TXE/RXNE poll loops compile to infinite loops (value cached)\n"
        "    - DR write may be eliminated (compiler sees no use of written val)\n"
        "    - CR1 multi-bit writes may be merged (intermediate states lost)\n"
        "  With volatile:\n"
        "    - Every access is a real load/store instruction to the bus\n"
        "    - Compiler cannot reorder accesses across volatile barriers\n"
        "    - Peripheral state machine sees correct timing\n"
    );
}
