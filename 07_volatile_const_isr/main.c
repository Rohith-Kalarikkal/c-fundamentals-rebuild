/**
 * @file    main.c
 * @brief   Entry point — runs all three programs.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include "platform_types.h"
#include "hw_registers.h"
#include "flash_const_data.h"
#include "isr_flags.h"

/* Forward declarations */
extern void volatile_registers(void);
extern void const_flash(void);
extern void run_isr_flags(void);

/* ------------------------------------------------------------------
 * print_separator
 * ------------------------------------------------------------------ */
static void print_separator(void)
{
    printf("\n------------------------------------------------------------------\n\n");
}

/* ------------------------------------------------------------------
 * main
 * ------------------------------------------------------------------*/
int main(void)
{
    printf("------------------------------------------------------------------\n");
    printf("               Embedded C: volatile & const & ISR                 \n");
    printf("------------------------------------------------------------------\n");

    /* Volatile hardware registers */
    volatile_registers();
    print_separator();

    /* Const in Flash memory */
    const_flash();
    print_separator();

    /* ISR volatile flags */
    run_isr_flags();
    print_separator();

    printf("All complete.\n");
    return 0;
}
