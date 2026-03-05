/**
 * @file    hw_registers.h
 * @brief   Simulated memory-mapped peripheral register definitions.
 *
 * On a real MCU (e.g., STM32, LPC55, TMS570) the linker script and
 * vendor header map peripheral base addresses to fixed physical addresses.
 * Here we allocate ordinary global structs in RAM and cast their addresses
 * to the same "register map" structs - giving identical C-level behaviour.
 *
 * Demonstrates:
 *  1. WHY every hardware register field is declared volatile.
 *  2. How a peripheral register map is modelled as a packed struct.
 *  3. Bit-field layout inside a 32-bit control register.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef HW_REGISTERS_H
#define HW_REGISTERS_H

#include "platform_types.h"

void volatile_registers(void);

/* ------------------------------------------------------------------
 * UART Peripheral Register Map
 *
 * Each field is volatile because:
 *   • The UART hardware changes SR (Status Register) bits asynchronously.
 *   • Writing DR (Data Register) triggers a hardware transmission.
 *   • Without volatile the compiler may merge, reorder, or eliminate
 *     reads/writes that look "redundant" from a pure software perspective.
 *
 * __attribute__((packed)) ensures no padding bytes are inserted between
 * fields - padding would shift register offsets and corrupt access.
 * ------------------------------------------------------------------ */
typedef struct __attribute__((packed))
{
    volatile uint32_t SR;   /* Offset 0x00 - Status Register  (R)   */
    volatile uint32_t DR;   /* Offset 0x04 - Data Register    (R/W) */
    volatile uint32_t BRR;  /* Offset 0x08 - Baud Rate Register (W) */
    volatile uint32_t CR1;  /* Offset 0x0C - Control Register 1 (W) */
    volatile uint32_t CR2;  /* Offset 0x10 - Control Register 2 (W) */
} UART_RegMap_t;

/* ------------------------------------------------------------------
 * SR – Status Register bit definitions (same as STM32F4 USART_SR)
 * ------------------------------------------------------------------ */
#define UART_SR_TXE     BIT(7)  /* Transmit Data Register Empty  */
#define UART_SR_TC      BIT(6)  /* Transmission Complete         */
#define UART_SR_RXNE    BIT(5)  /* Read Data Register Not Empty  */
#define UART_SR_ORE     BIT(3)  /* Overrun Error                 */

/* ------------------------------------------------------------------
 * CR1 – Control Register 1 bit definitions
 * ------------------------------------------------------------------ */
#define UART_CR1_UE     BIT(13) /* UART Enable                   */
#define UART_CR1_TXEIE  BIT(7)  /* TXE Interrupt Enable          */
#define UART_CR1_RXNEIE BIT(5)  /* RXNE Interrupt Enable         */
#define UART_CR1_TE     BIT(3)  /* Transmitter Enable            */
#define UART_CR1_RE     BIT(2)  /* Receiver Enable               */

/* ------------------------------------------------------------------
 * GPIO Peripheral Register Map
 * ------------------------------------------------------------------ */
typedef struct __attribute__((packed))
{
    volatile uint32_t MODER;    /* Offset 0x00 – Mode Register           */
    volatile uint32_t OTYPER;   /* Offset 0x04 – Output Type Register    */
    volatile uint32_t OSPEEDR;  /* Offset 0x08 – Output Speed Register   */
    volatile uint32_t PUPDR;    /* Offset 0x0C – Pull-up/Pull-down Reg   */
    volatile uint32_t IDR;      /* Offset 0x10 – Input Data Register (R) */
    volatile uint32_t ODR;      /* Offset 0x14 – Output Data Register(W) */
    volatile uint32_t BSRR;     /* Offset 0x18 – Bit Set/Reset Register  */
    volatile uint32_t LCKR;     /* Offset 0x1C – Lock Register           */
} GPIO_RegMap_t;

/* ------------------------------------------------------------------
 * Simulated base-address "instances"
 *
 * On real hardware:
 *   #define UART1  ((UART_RegMap_t *) 0x40011000UL)
 *
 * Here we expose pointers that hw_registers.c points at simulated RAM.
 * ------------------------------------------------------------------ */
extern UART_RegMap_t  *const SIM_UART1;
extern GPIO_RegMap_t  *const SIM_GPIOA;

/* ------------------------------------------------------------------
 * Peripheral initialisation / simulation API
 * ------------------------------------------------------------------ */

/**
 * @brief  Initialise simulated peripheral instances.
 *         Call once at program start before any other peripheral access.
 */
void hw_registers_init(void);

/**
 * @brief  Simulate one "hardware tick": the peripheral state machine runs,
 *         toggling status bits as real hardware would.
 * @param  uart   Pointer to the UART register map to simulate.
 */
void hw_uart_simulate_tick(UART_RegMap_t *uart);

/**
 * @brief  Print a human-readable dump of UART register state.
 * @param  uart   Pointer to the UART register map.
 * @param  label  Short label printed in the header line.
 */
void hw_uart_dump(const UART_RegMap_t *uart, const char *label);

#endif /* HW_REGISTERS_H */
