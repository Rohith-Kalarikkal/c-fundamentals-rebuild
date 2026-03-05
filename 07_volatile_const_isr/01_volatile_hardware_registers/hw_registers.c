/**
 * @file    hw_registers.c
 * @brief   Simulated memory-mapped hardware register implementation.
 *
 * This module allocates plain global structs (in SRAM on a real MCU, in
 * process memory on a host PC) and exposes them as volatile register maps.
 * The simulation tick function mimics what the peripheral state machine
 * would do on real hardware.
 *
 * KEY LESSON — volatile prevents these real compiler optimisations:
 *
 *   (a) Dead-store elimination:
 *       Writing UART1->DR twice in a row - the compiler drops the first write
 *       for a non-volatile variable because "nobody reads it". On real HW
 *       both writes kick off a hardware transmission.
 *
 *   (b) Load hoisting:
 *       Reading UART1->SR in a tight polling loop - without volatile the
 *       compiler reads SR once before the loop and reuses the register copy.
 *       The TXE bit never appears to change. Classic embedded hang.
 *
 *   (c) Store sinking / merging:
 *       Two successive writes to CR1 may be merged into one. On hardware,
 *       intermediate states can matter (e.g., enabling TX before RX).
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "hw_registers.h"
#include <stdio.h>
#include <string.h>

/* --------------------------------------------------------------------------
 * Private: backing storage for simulated peripherals
 *
 * On a real MCU these addresses would be fixed by the silicon vendor.
 * Here we allocate them as globals and hand out pointers.
 * -------------------------------------------------------------------------- */
static UART_RegMap_t  s_uart1_regs;   /* Backing memory for SIM_UART1  */
static GPIO_RegMap_t  s_gpioa_regs;   /* Backing memory for SIM_GPIOA  */

/* --------------------------------------------------------------------------
 * Public const pointers (the "base address" of each peripheral)
 *
 * const on the pointer itself: the base address never changes at runtime,
 * just like a hardware base address defined by silicon. The pointed-to
 * registers are volatile - they change independently of C code.
 * -------------------------------------------------------------------------- */
UART_RegMap_t  *const SIM_UART1  = &s_uart1_regs;
GPIO_RegMap_t  *const SIM_GPIOA  = &s_gpioa_regs;

/* --------------------------------------------------------------------------
 * hw_registers_init
 * -------------------------------------------------------------------------- */
void hw_registers_init(void)
{
    /* Zero-initialise - mimics MCU reset state (all registers = 0x00000000). */
    memset(&s_uart1_regs, 0, sizeof(s_uart1_regs));
    memset(&s_gpioa_regs, 0, sizeof(s_gpioa_regs));

    /*
     * After reset, STM32-style UART SR has TXE and TC set because the
     * transmit buffer is empty. We replicate that behaviour here.
     */
    SIM_UART1->SR = UART_SR_TXE | UART_SR_TC;

    /*
     * Configure UART: enable peripheral, TX, RX.
     * Note: we write CR1 once - on real HW order of bit-setting can matter.
     */
    SIM_UART1->CR1 = UART_CR1_UE | UART_CR1_TE | UART_CR1_RE;
    SIM_UART1->BRR = 0x0683U;  /* 115200 baud @ 84 MHz APB2 (STM32F4 value) */

    printf("[HW_INIT] UART1 registers initialised. SR=0x%08X CR1=0x%08X\n",
           (unsigned)SIM_UART1->SR, (unsigned)SIM_UART1->CR1);
}

/* --------------------------------------------------------------------------
 * hw_uart_simulate_tick
 *
 * Pretends to be the UART state machine running for one "clock cycle":
 *   - If someone wrote a byte to DR, clear TXE (buffer full), then after
 *     one more tick set TXE again (byte shifted out to wire) and set TC.
 *   - If there is incoming data pending, set RXNE.
 * -------------------------------------------------------------------------- */

/** Internal: tracks whether a byte is in flight in the TX shift register. */
static bool s_tx_in_flight = false;

void hw_uart_simulate_tick(UART_RegMap_t *uart)
{
    if (uart == NULL) { return; }

    if (s_tx_in_flight)
    {
        /*
         * Simulate the byte finishing transmission.
         * Real hardware asserts TC and TXE after the stop bit.
         */
        SET_BIT(uart->SR, UART_SR_TXE);
        SET_BIT(uart->SR, UART_SR_TC);
        s_tx_in_flight = false;
        printf("[HW_TICK] TX complete. SR=0x%08X\n", (unsigned)uart->SR);
    }
    else if (!TST_BIT(uart->SR, UART_SR_TXE))
    {
        /*
         * TXE was cleared (caller wrote to DR) - byte is now in flight.
         * Clear TC; real HW does this when a new transmission starts.
         */
        CLR_BIT(uart->SR, UART_SR_TC);
        s_tx_in_flight = true;
        printf("[HW_TICK] TX in-flight (byte being shifted out). SR=0x%08X\n",
               (unsigned)uart->SR);
    }
}

/* --------------------------------------------------------------------------
 * hw_uart_dump
 * -------------------------------------------------------------------------- */
void hw_uart_dump(const UART_RegMap_t *uart, const char *label)
{
    if (uart == NULL || label == NULL) { return; }

    printf("\n---- UART Register Dump: %s ---------------------\n", label);
    printf("  SR  = 0x%08X  [TXE=%u TC=%u RXNE=%u ORE=%u\n",
           (unsigned)uart->SR,
           (unsigned)TST_BIT(uart->SR, UART_SR_TXE),
           (unsigned)TST_BIT(uart->SR, UART_SR_TC),
           (unsigned)TST_BIT(uart->SR, UART_SR_RXNE),
           (unsigned)TST_BIT(uart->SR, UART_SR_ORE));
    printf("  DR  = 0x%08X  (last byte = '%c')\n",
           (unsigned)uart->DR,
           (uart->DR >= 0x20U && uart->DR <= 0x7EU) ? (char)uart->DR : '.');
    printf("  BRR = 0x%08X\n", (unsigned)uart->BRR);
    printf("  CR1 = 0x%08X  [UE=%u TE=%u RE=%u RXNEIE=%u TXEIE=%u]\n",
           (unsigned)uart->CR1,
           (unsigned)TST_BIT(uart->CR1, UART_CR1_UE),
           (unsigned)TST_BIT(uart->CR1, UART_CR1_TE),
           (unsigned)TST_BIT(uart->CR1, UART_CR1_RE),
           (unsigned)TST_BIT(uart->CR1, UART_CR1_RXNEIE),
           (unsigned)TST_BIT(uart->CR1, UART_CR1_TXEIE));
    printf("--------------------------------------------------------------------------\n\n");
}
