/**
 * @file    flash_const_data.c
 * @brief   const-qualified data placed in Flash (ROM) — implementation.
 *
 * All objects here are declared const so the linker places them in the
 * .rodata section which maps to Flash. On a host build they live in the
 * read-only data segment of the process - writes cause a SIGSEGV, exactly
 * like writing to Flash from software on an MCU.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "flash_const_data.h"
#include <stdio.h>
#include <math.h>   /* sin(), M_PI - used once to generate the LUT comment  */
#include <string.h>

/* ------------------------------------------------------------------------
 * Sine look-up table — 256 samples, Q0.12 (0–4095), quarter-wave.
 *
 * In a real product this table would be pre-computed offline and placed here
 * as a const array. Computing sin() in firmware wastes Flash and CPU.
 * Placing it const saves SRAM: the linker keeps it in Flash and the CPU
 * fetches samples directly from there via the I-cache (Cortex-M3/M4/M7).
 * ------------------------------------------------------------------------ */
FLASH_DATA uint16_t g_sine_lut[256] =
{
    /* Pre-computed: value[i] = (uint16_t)round(sin(i * pi / 512) * 4095) */
    /*   0 */    0,   25,   50,   75,  100,  125,  150,  175,
    /*   8 */  200,  225,  251,  276,  301,  326,  351,  376,
    /*  16 */  401,  426,  451,  476,  501,  526,  551,  576,
    /*  24 */  601,  626,  650,  675,  700,  725,  750,  775,
    /*  32 */  799,  824,  848,  873,  897,  922,  946,  971,
    /*  40 */  995, 1019, 1044, 1068, 1092, 1116, 1140, 1164,
    /*  48 */ 1188, 1212, 1236, 1260, 1284, 1307, 1331, 1354,
    /*  56 */ 1378, 1401, 1424, 1448, 1471, 1494, 1517, 1540,
    /*  64 */ 1563, 1585, 1608, 1631, 1653, 1676, 1698, 1720,
    /*  72 */ 1742, 1764, 1786, 1808, 1830, 1852, 1874, 1895,
    /*  80 */ 1917, 1938, 1959, 1980, 2001, 2022, 2043, 2064,
    /*  88 */ 2085, 2105, 2126, 2146, 2166, 2186, 2206, 2226,
    /*  96 */ 2246, 2265, 2285, 2304, 2323, 2342, 2362, 2380,
    /* 104 */ 2399, 2418, 2436, 2455, 2473, 2491, 2509, 2527,
    /* 112 */ 2545, 2563, 2580, 2598, 2615, 2632, 2649, 2666,
    /* 120 */ 2683, 2700, 2716, 2732, 2749, 2765, 2780, 2796,
    /* 128 */ 2811, 2827, 2842, 2857, 2872, 2887, 2901, 2916,
    /* 136 */ 2930, 2944, 2958, 2972, 2986, 2999, 3013, 3026,
    /* 144 */ 3039, 3052, 3065, 3077, 3090, 3102, 3114, 3126,
    /* 152 */ 3138, 3150, 3161, 3173, 3184, 3195, 3206, 3217,
    /* 160 */ 3227, 3238, 3248, 3258, 3268, 3278, 3287, 3297,
    /* 168 */ 3306, 3315, 3324, 3333, 3341, 3350, 3358, 3366,
    /* 176 */ 3374, 3382, 3390, 3397, 3405, 3412, 3419, 3426,
    /* 184 */ 3432, 3439, 3445, 3451, 3457, 3463, 3469, 3474,
    /* 192 */ 3480, 3485, 3490, 3494, 3499, 3503, 3508, 3512,
    /* 200 */ 3516, 3519, 3523, 3526, 3530, 3533, 3536, 3538,
    /* 208 */ 3541, 3543, 3546, 3548, 3550, 3551, 3553, 3554,
    /* 216 */ 3556, 3557, 3558, 3559, 3559, 3560, 3560, 3560,
    /* 224 */ 3560, 3560, 3560, 3560, 3559, 3559, 3558, 3557,
    /* 232 */ 3556, 3555, 3553, 3552, 3550, 3548, 3546, 3544,
    /* 240 */ 3541, 3539, 3536, 3533, 3530, 3527, 3524, 3520,
    /* 248 */ 3517, 3513, 3509, 3505, 3501, 3496, 3492, 3487
};

/* ------------------------------------------------------------------------
 * Firmware version — const struct lives in Flash (.rodata)
 * ------------------------------------------------------------------------ */
FLASH_DATA FwVersion_t g_fw_version =
{
    .major      = 2U,
    .minor      = 4U,
    .patch      = 1U,
    .build_date = __DATE__,   /* Replaced by compiler with e.g. "Mar 04 2026" */
    .build_time = __TIME__
};

/* ------------------------------------------------------------------------
 * Error table — array of structs in Flash
 *
 * String literals ("Overrun error", …) are ALREADY placed in .rodata by
 * the compiler. The pointers in the struct just reference those locations.
 * ------------------------------------------------------------------------ */
FLASH_DATA ErrorEntry_t g_error_table[] =
{
    { 0U, "No error"                            },
    { 1U, "Generic error"                       },
    { 2U, "Peripheral busy"                     },
    { 3U, "Timeout waiting for hardware ready"  },
    { 4U, "UART overrun — byte dropped"         },
    { 5U, "I2C NACK received from slave"        },
    { 6U, "SPI frame error"                     },
    { 7U, "Flash write protection violation"    },
    { 8U, "DMA transfer error"                  },
    { 9U, "Watchdog reset triggered"            }
};

FLASH_DATA uint8_t g_error_table_size =
    (uint8_t)(sizeof(g_error_table) / sizeof(g_error_table[0]));

/* ------------------------------------------------------------------------
 * Simulated Device-ID registers (read-only hardware perspective)
 * ------------------------------------------------------------------------ */
static DeviceID_RegMap_t s_device_id_regs;
DeviceID_RegMap_t *const SIM_DEVICE_ID = &s_device_id_regs;

/* ------------------------------------------------------------------------
 * flash_const_init
 * ------------------------------------------------------------------------ */
void flash_const_init(void)
{
    /*
     * In production this function would be empty - const data is baked into
     * Flash at link time. For the simulation we cast away const ONCE here
     * (in the BSP init, not in application code) to seed the device-ID
     * registers, just as a bootloader would write them during manufacturing.
     *
     * Casting away const is a deliberate, documented exception - never do
     * this in application-layer code.
     */
    const uint32_t seed[5] = {0x04350001UL,0x00000003UL,0xDEADBEEFUL,0xCAFEBABEUL,0x12345678UL};
    volatile uint32_t *raw = (volatile uint32_t *)(uintptr_t)SIM_DEVICE_ID;
    for (size_t i = 0U; i < 5U; i++) { raw[i] = seed[i]; }

    printf("[FLASH_INIT] Device-ID registers seeded (BSP init only).\n");
}

/* ------------------------------------------------------------------------
 * flash_print_fw_version
 * ------------------------------------------------------------------------ */
void flash_print_fw_version(void)
{
    printf("\n  Firmware Version : %u.%u.%u\n",
           g_fw_version.major,
           g_fw_version.minor,
           g_fw_version.patch);
    printf("  Build Date       : %s\n", g_fw_version.build_date);
    printf("  Build Time       : %s\n", g_fw_version.build_time);
    printf("  (g_fw_version lives at address %p - in .rodata / Flash)\n",
           (const void *)&g_fw_version);
}

/* ------------------------------------------------------------------------
 * flash_sine_lut
 * ------------------------------------------------------------------------ */
void flash_sine_lut(void)
{
    printf("\n  Sine LUT (first 8 / last 8 samples):\n");
    printf("  Idx   Value (Q0.12)\n");
    printf("  --------------------\n");

    for (uint32_t i = 0U; i < 8U; i++)
    {
        printf("  [%3u]  %4u\n", (unsigned)i, (unsigned)g_sine_lut[i]);
    }
    printf("  ...\n");
    for (uint32_t i = 248U; i < 256U; i++)
    {
        printf("  [%3u]  %4u\n", (unsigned)i, (unsigned)g_sine_lut[i]);
    }

    printf("\n  LUT address: %p (ROM / .rodata - writes would fault on HW)\n",
           (const void *)g_sine_lut);
}

/* ------------------------------------------------------------------------
 * flash_device_id
 * ------------------------------------------------------------------------ */
void flash_device_id(void)
{
    printf("\n  Device ID Registers (volatile const - HW read-only):\n");
    printf("  CHIP_ID    = 0x%08X\n", (unsigned)SIM_DEVICE_ID->CHIP_ID);
    printf("  REVISION   = 0x%08X\n", (unsigned)SIM_DEVICE_ID->REVISION);
    printf("  UNIQUE_ID  = %08X-%08X-%08X\n",
           (unsigned)SIM_DEVICE_ID->UNIQUE_ID0,
           (unsigned)SIM_DEVICE_ID->UNIQUE_ID1,
           (unsigned)SIM_DEVICE_ID->UNIQUE_ID2);

    printf("\n  NOTE: 'volatile const' means:\n");
    printf("    volatile -> compiler must re-read from HW every access\n");
    printf("    const    -> software is forbidden to write (bus error on HW)\n");
}

/* ------------------------------------------------------------------------
 * flash_get_error_str
 * ------------------------------------------------------------------------ */
const char *flash_get_error_str(uint8_t code)
{
    for (uint8_t i = 0U; i < g_error_table_size; i++)
    {
        if (g_error_table[i].code == code)
        {
            return g_error_table[i].description;
        }
    }
    return "Unknown error code";
}
