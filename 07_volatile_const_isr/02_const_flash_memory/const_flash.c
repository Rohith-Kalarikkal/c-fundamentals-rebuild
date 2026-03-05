/**
 * @file    const_flash.c
 * @brief   Demonstrates const data in Flash (ROM) and volatile const registers.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "flash_const_data.h"
#include <stdio.h>

void const_flash(void)
{
    printf("\n--------------------------------------------------------------------------\n");
    printf("                          const in Flash Memory                             \n");
    printf("--------------------------------------------------------------------------\n");

    flash_const_init();

    /* --- Firmware version ----------------------------------------------- */
    printf("\n--- Firmware version (const struct in .rodata) ------------------\n");
    flash_print_fw_version();

    /* ---Sine LUT --------------------------------------------------------- */
    printf("\n--- Sine LUT (const uint16_t[256] in .rodata) -------------------\n");
    flash_sine_lut();

    /* --- Device ID (volatile const registers) ---------------------------- */
    printf("\n--- Device-ID registers (volatile const) ------------------------\n");
    flash_device_id();

    /* --- Error table ---------------------------------------------------- */
    printf("\n--- Error string lookup from Flash table ------------------------\n");
    for (uint8_t code = 0U; code <= 9U; code++)
    {
        printf("  Error %u -> \"%s\"\n", (unsigned)code, flash_get_error_str(code));
    }

    /* --- const lesson summary ------------------------------------------ */
    printf("\n--- const lesson summary ---------------------------------------\n");
    printf(
        "  const placed before variable type:\n"
        "    - Linker emits symbol to .rodata (-> Flash on MCU)\n"
        "    - Saves scarce SRAM - LUTs, version structs, error tables\n"
        "    - Compiler error on accidental writes -> safer code\n"
        "    - Enables string literal sharing / de-duplication\n"
        "\n"
        "  volatile const combination:\n"
        "    - volatile -> compiler cannot cache, every read hits the bus\n"
        "    - const    -> application code cannot write (HW-enforced RO)\n"
        "    - Ideal for: chip-ID reg, read-only status registers, eFuse\n"
    );
}
