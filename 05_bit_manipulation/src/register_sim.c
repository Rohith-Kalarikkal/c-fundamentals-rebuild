/**
 * @file    register_sim.c
 * @brief   Simulated peripheral register-control implementation.
 *
 * This file implements a simulated embedded peripheral that mimics
 * the register-level programming model found on real MCU peripherals
 * (UART, SPI, GPIO, timers, etc.).
 *
 * Key design points that mirror real firmware:
 *   1. Every bit position is named — never use magic numbers directly.
 *   2. Macros (SET_BIT, CLEAR_BIT, etc.) are the ONLY way to mutate bits.
 *   3. Hardware-set bits (STATUS_REG) are never written by firmware.
 *   4. All registers are volatile (important on real targets with HW).
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "register_sim.h"
#include "bit_ops.h"

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                  */
/* ------------------------------------------------------------------ */

/** Width of the register-dump label column */
#define DUMP_LABEL_WIDTH    28

/**
 * @brief  Render one register row in the dump table.
 */
static void dump_reg_row(const char *name, uint32_t value)
{
    char binstr[40];
    /* Re-use the internal binstr builder - declared static here to
     * avoid exposing it outside this translation unit.               
     */
    uint8_t out_idx = 0;
    int8_t  i;
    for (i = 31; i >= 0; --i) {
        binstr[out_idx++] = (char)('0' + ((value >> i) & 1UL));
        if ((i > 0) && (i % 4 == 0)) {
            binstr[out_idx++] = '_';
        }
    }
    binstr[out_idx] = '\0';

    printf("  %-*s : 0b %s  (0x%08X)\n",
           DUMP_LABEL_WIDTH, name, binstr, value);
}

/* ------------------------------------------------------------------ */
/*  Public API — implementation                                        */
/* ------------------------------------------------------------------ */

void periph_init(Peripheral_RegMap_t *periph)
{
    if (periph == NULL) {
        return; /* Guard against null pointer — always check in embedded */
    }

    /*
     * On real hardware this would be a software-reset sequence.
     * Here we simply zero all registers to match the post-reset state
     * documented in the (hypothetical) datasheet.
     */
    WRITE_REG(periph->CTRL_REG,   0x00000000UL);
    WRITE_REG(periph->STATUS_REG, 0x00000000UL);
    WRITE_REG(periph->DATA_REG,   0x00000000UL);

    printf("  [periph_init]  Registers reset to 0x00000000\n");
}

void periph_enable(Peripheral_RegMap_t *periph)
{
    /*
     * SET_BIT expands to:
     *   periph->CTRL_REG |= (1UL << CTRL_ENABLE_BIT);
     */
    SET_BIT(periph->CTRL_REG, CTRL_ENABLE_BIT);
    printf("  [periph_enable]  CTRL_ENABLE_BIT set\n");
}

void periph_disable(Peripheral_RegMap_t *periph)
{
    /*
     * CLEAR_BIT expands to:
     *   periph->CTRL_REG &= ~(1UL << CTRL_ENABLE_BIT);
     */
    CLEAR_BIT(periph->CTRL_REG, CTRL_ENABLE_BIT);
    printf("  [periph_disable] CTRL_ENABLE_BIT cleared\n");
}

void periph_set_mode(Peripheral_RegMap_t *periph, bool tx_mode)
{
    if (tx_mode) {
        SET_BIT(periph->CTRL_REG, CTRL_MODE_BIT);
        printf("  [periph_set_mode] Mode -> TX (bit 1 = 1)\n");
    } else {
        CLEAR_BIT(periph->CTRL_REG, CTRL_MODE_BIT);
        printf("  [periph_set_mode] Mode -> RX (bit 1 = 0)\n");
    }
}

void periph_set_irq(Peripheral_RegMap_t *periph, bool enable)
{
    if (enable) {
        SET_BIT(periph->CTRL_REG, CTRL_IRQ_ENABLE_BIT);
        printf("  [periph_set_irq]  Interrupt ENABLED  (bit 2 = 1)\n");
    } else {
        CLEAR_BIT(periph->CTRL_REG, CTRL_IRQ_ENABLE_BIT);
        printf("  [periph_set_irq]  Interrupt DISABLED (bit 2 = 0)\n");
    }
}

void periph_write_data(Peripheral_RegMap_t *periph,
                       uint8_t channel, uint8_t payload)
{
    /*
     * Build DATA_REG in a single write:
     *   Bits [7:0]  = payload (8-bit data)
     *   Bits [11:8] = channel (4-bit ID)
     *
     * We use SET_FIELD which handles masking and shifting:
     *   SET_FIELD(REG, POS, WIDTH, VAL)
     */
    WRITE_REG(periph->DATA_REG, 0UL); /* Clear first */
    SET_FIELD(periph->DATA_REG, DATA_PAYLOAD_POS, DATA_PAYLOAD_WIDTH, payload);
    SET_FIELD(periph->DATA_REG, DATA_CHANNEL_POS, DATA_CHANNEL_WIDTH, channel);

    printf("  [periph_write_data] ch=%u  data=0x%02X -> DATA_REG=0x%08X\n",
           channel, payload, periph->DATA_REG);
}

void periph_sim_hw_event(Peripheral_RegMap_t *periph, uint8_t flag_bit)
{
    /*
     * Simulate the hardware asserting a STATUS flag.
     * In real code firmware would NOT write status bits; the MCU
     * peripheral does it automatically.  We break that rule here
     * only to drive the program without real hardware.
     */
    SET_BIT(periph->STATUS_REG, flag_bit);
    printf("  [hw_event] STATUS bit %u asserted  (STATUS_REG=0x%08X)\n",
           flag_bit, periph->STATUS_REG);
}

void periph_dump_registers(const Peripheral_RegMap_t *periph)
{
    printf("\n--------------------------------------------- \n");
    printf("         PERIPHERAL REGISTER DUMP               \n");
    printf("  --------------------------------------------- \n");

    dump_reg_row("CTRL_REG   [7:0]", periph->CTRL_REG);

    /* Decode CTRL_REG bits individually */
    printf("      Bit 0 ENABLE    = %lu\n", (unsigned long)CHECK_BIT(periph->CTRL_REG, CTRL_ENABLE_BIT));
    printf("      Bit 1 MODE      = %lu  (%s)\n",
           (unsigned long)CHECK_BIT(periph->CTRL_REG, CTRL_MODE_BIT),
           CHECK_BIT(periph->CTRL_REG, CTRL_MODE_BIT) ? "TX" : "RX");
    printf("      Bit 2 IRQ_EN    = %lu\n", (unsigned long)CHECK_BIT(periph->CTRL_REG, CTRL_IRQ_ENABLE_BIT));
    printf("      Bit 3 LOOPBACK  = %lu\n", (unsigned long)CHECK_BIT(periph->CTRL_REG, CTRL_LOOPBACK_BIT));
    printf("      Bit 7 RESET     = %lu\n", (unsigned long)CHECK_BIT(periph->CTRL_REG, CTRL_RESET_BIT));

    printf("  ----------------------------------------------------------------\n");
    dump_reg_row("STATUS_REG [7:0]", periph->STATUS_REG);
    printf("      Bit 0 BUSY      = %lu\n", (unsigned long)CHECK_BIT(periph->STATUS_REG, STATUS_BUSY_BIT));
    printf("      Bit 1 TX_EMPTY  = %lu\n", (unsigned long)CHECK_BIT(periph->STATUS_REG, STATUS_TX_EMPTY_BIT));
    printf("      Bit 2 RX_FULL   = %lu\n", (unsigned long)CHECK_BIT(periph->STATUS_REG, STATUS_RX_FULL_BIT));
    printf("      Bit 3 OVERFLOW  = %lu\n", (unsigned long)CHECK_BIT(periph->STATUS_REG, STATUS_OVERFLOW_BIT));
    printf("      Bit 4 PARITY_ER = %lu\n", (unsigned long)CHECK_BIT(periph->STATUS_REG, STATUS_PARITY_ERR_BIT));

    printf("  ----------------------------------------------------------------\n");
    dump_reg_row("DATA_REG  [11:0]", periph->DATA_REG);
    printf("      Bits[7:0]  PAYLOAD = 0x%02lX (%lu)\n",
           (unsigned long)GET_FIELD(periph->DATA_REG, DATA_PAYLOAD_POS, DATA_PAYLOAD_WIDTH),
           (unsigned long)GET_FIELD(periph->DATA_REG, DATA_PAYLOAD_POS, DATA_PAYLOAD_WIDTH));
    printf("      Bits[11:8] CHANNEL = %lu\n",
           (unsigned long)GET_FIELD(periph->DATA_REG, DATA_CHANNEL_POS, DATA_CHANNEL_WIDTH));

    printf("  ----------------------------------------------------------------\n");
}

/* ------------------------------------------------------------------ */
/*  Full sequence                                                 */
/* ------------------------------------------------------------------ */

void register_simulation(void)
{
    Peripheral_RegMap_t myPeriph; /* Lives on the stack for this demo   */

    printf("\n");
    printf("--------------------------------------------------------\n");
    printf("        Simulated Peripheral Register Control           \n");
    printf("--------------------------------------------------------\n");

    /* Step 1: Initialise */
    printf("\n  Step 1: Initialise peripheral\n");
    periph_init(&myPeriph);

    /* Step 2: Enable */
    printf("\n  Step 2: Enable peripheral\n");
    periph_enable(&myPeriph);

    /* Step 3: Set TX mode */
    printf("\n  Step 3: Set TX mode\n");
    periph_set_mode(&myPeriph, true);

    /* Step 4: Enable interrupt */
    printf("\n  Step 4: Enable interrupt\n");
    periph_set_irq(&myPeriph, true);

    /* Step 5: Register snapshot after config */
    printf("\n  Step 5: Register snapshot after configuration\n");
    periph_dump_registers(&myPeriph);

    /* Step 6: Transmit a data byte on channel 3 */
    printf("\n  Step 6: Write data byte 0xA5 on channel 3\n");
    periph_write_data(&myPeriph, 3U, 0xA5U);

    /* Step 7: Simulate HW setting BUSY and TX_EMPTY flags */
    printf("\n  Step 7: Hardware asserts BUSY flag\n");
    periph_sim_hw_event(&myPeriph, STATUS_BUSY_BIT);

    printf("\n  Step 8: Hardware asserts TX_EMPTY (transfer complete)\n");
    periph_sim_hw_event(&myPeriph, STATUS_TX_EMPTY_BIT);

    /* Firmware would poll or be interrupted here.
     * Let's simulate clearing the busy flag (HW does this, simulated): */
    CLEAR_BIT(myPeriph.STATUS_REG, STATUS_BUSY_BIT);
    printf("  [fw poll] BUSY cleared - transfer finished\n");

    /* Step 9: Toggle loopback for diagnostic mode */
    printf("\n  Step 9: Toggle LOOPBACK bit for diagnostics\n");
    TOGGLE_BIT(myPeriph.CTRL_REG, CTRL_LOOPBACK_BIT);
    printf("  LOOPBACK = %lu  (now active)\n",
           (unsigned long)CHECK_BIT(myPeriph.CTRL_REG, CTRL_LOOPBACK_BIT));
    TOGGLE_BIT(myPeriph.CTRL_REG, CTRL_LOOPBACK_BIT);
    printf("  LOOPBACK = %lu  (toggled off)\n",
           (unsigned long)CHECK_BIT(myPeriph.CTRL_REG, CTRL_LOOPBACK_BIT));

    /* Step 10: Disable peripheral when done */
    printf("\n  Step 10: Disable peripheral\n");
    periph_disable(&myPeriph);

    /* Final register snapshot */
    printf("\n  Final register snapshot:\n");
    periph_dump_registers(&myPeriph);
}
