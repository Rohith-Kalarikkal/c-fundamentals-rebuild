/**
 * @file    register_sim.h
 * @brief   Simulated peripheral register map and control API.
 *
 * Models a simplified MCU-style peripheral with:
 *   - Control Register  (CTRL_REG)  — enable, mode, interrupt flags
 *   - Status Register   (STATUS_REG)— read-only hardware flags
 *   - Data Register     (DATA_REG)  — 8-bit payload
 *
 * The register layout below mimics a real embedded workflow where
 * engineers define bit positions as named constants, then use the
 * SET_BIT / CLEAR_BIT / CHECK_BIT macros from bit_ops.h to drive them.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef REGISTER_SIM_H
#define REGISTER_SIM_H

#include <stdint.h>
#include <stdbool.h>
#include "bit_ops.h"

/* ------------------------------------------------------------------ */
/*  Simulated peripheral register block                               */
/* ------------------------------------------------------------------ */

/**
 * @brief  In-memory representation of a peripheral's register set.
 *
 *  Real hardware: these would be volatile uint32_t mapped to fixed
 *  memory addresses via linker script or #define to a raw address.
 *  Example (STM32 style):
 *    #define GPIOA_ODR  (*(volatile uint32_t *)0x40020014U)
 */
typedef struct {
    volatile uint32_t CTRL_REG;    /* Control register                  */
    volatile uint32_t STATUS_REG;  /* Status register  (hw sets these)  */
    volatile uint32_t DATA_REG;    /* Data register    (8-bit payload)   */
} Peripheral_RegMap_t;

/* ------------------------------------------------------------------ */
/*  Control Register (CTRL_REG) bit positions                          */
/* ------------------------------------------------------------------ */

#define CTRL_ENABLE_BIT         0U  /* Bit 0: Peripheral enable (1=ON)  */
#define CTRL_MODE_BIT           1U  /* Bit 1: Mode select (0=RX, 1=TX)  */
#define CTRL_IRQ_ENABLE_BIT     2U  /* Bit 2: Interrupt enable           */
#define CTRL_LOOPBACK_BIT       3U  /* Bit 3: Loopback test mode         */
#define CTRL_RESET_BIT          7U  /* Bit 7: Software reset (self-clr)  */

/* ------------------------------------------------------------------ */
/* Status Register (STATUS_REG) bit positions — read-only by firmware */
/* ------------------------------------------------------------------ */

#define STATUS_BUSY_BIT         0U  /* Bit 0: Peripheral busy            */
#define STATUS_TX_EMPTY_BIT     1U  /* Bit 1: TX buffer empty            */
#define STATUS_RX_FULL_BIT      2U  /* Bit 2: RX buffer full             */
#define STATUS_OVERFLOW_BIT     3U  /* Bit 3: Data overflow error        */
#define STATUS_PARITY_ERR_BIT   4U  /* Bit 4: Parity error               */

/* ------------------------------------------------------------------ */
/*  Data Register (DATA_REG) field layout                              */
/* ------------------------------------------------------------------ */

#define DATA_PAYLOAD_POS        0U  /* Bits [7:0]  - 8-bit data payload  */
#define DATA_PAYLOAD_WIDTH      8U
#define DATA_CHANNEL_POS        8U  /* Bits [11:8] - 4-bit channel ID    */
#define DATA_CHANNEL_WIDTH      4U

/* ------------------------------------------------------------------ */
/*  Public API                                                         */
/* ------------------------------------------------------------------ */

/**
 * @brief  Initialise the simulated peripheral to a known reset state.
 *
 * @param  periph  Pointer to the register map to initialise.
 */
void periph_init(Peripheral_RegMap_t *periph);

/**
 * @brief  Enable the peripheral (sets CTRL_ENABLE_BIT).
 *
 * @param  periph  Pointer to the register map.
 */
void periph_enable(Peripheral_RegMap_t *periph);

/**
 * @brief  Disable the peripheral (clears CTRL_ENABLE_BIT).
 *
 * @param  periph  Pointer to the register map.
 */
void periph_disable(Peripheral_RegMap_t *periph);

/**
 * @brief  Configure TX or RX mode.
 *
 * @param  periph  Pointer to the register map.
 * @param  tx_mode true  - set TX mode (bit = 1)
 *                 false - set RX mode (bit = 0)
 */
void periph_set_mode(Peripheral_RegMap_t *periph, bool tx_mode);

/**
 * @brief  Enable or disable the interrupt line.
 *
 * @param  periph  Pointer to the register map.
 * @param  enable  true to enable, false to disable.
 */
void periph_set_irq(Peripheral_RegMap_t *periph, bool enable);

/**
 * @brief  Transmit a byte on a given channel (writes DATA_REG fields).
 *
 * @param  periph   Pointer to the register map.
 * @param  channel  4-bit channel ID (0–15).
 * @param  payload  8-bit data byte to transmit.
 */
void periph_write_data(Peripheral_RegMap_t *periph,
                       uint8_t channel, uint8_t payload);

/**
 * @brief  Simulate a hardware event: inject a status flag as if the
 *         hardware had set it (for demonstration purposes only).
 *
 * @param  periph    Pointer to the register map.
 * @param  flag_bit  Bit position in STATUS_REG to assert.
 */
void periph_sim_hw_event(Peripheral_RegMap_t *periph, uint8_t flag_bit);

/**
 * @brief  Print the full register map in a formatted, annotated layout.
 *
 * @param  periph  Pointer to the register map to dump.
 */
void periph_dump_registers(const Peripheral_RegMap_t *periph);

/**
 * @brief  Run a complete register-control demo sequence.
 */
void register_simulation(void);

#endif /* REGISTER_SIM_H */
