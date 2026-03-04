/**
 * @file    src/memory_sections.c
 * @brief   Demonstrates how the linker assigns variables to ELF sections.
 *
 * Section placement rules (System V ELF / GNU ld)
 * ------------------------------------------------
 *  Variable type                         Section
 *  ─────────────────────────────────── ───────────
 *  const global (non-zero)              .rodata
 *  non-const global, non-zero init      .data
 *  non-const global, zero / no init     .bss
 *  local (automatic)                    stack  (no ELF section)
 *  local static, non-zero init          .data
 *  local static, zero / no init         .bss
 *  string literal                       .rodata  (on most toolchains)
 *  function body                        .text
 *
 * Why .bss saves flash space on embedded targets
 * -----------------------------------------------
 * .bss variables are NOT stored in the ELF/binary image.  The C runtime
 * startup (crt0 / __startup) zero-fills the .bss region before main().
 * This is critical on MCUs where flash (program memory) is scarce.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "memory_sections.h"
#include "alignment.h"    /* IN_DATA, IN_BSS, IN_RODATA macros              */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* ==========================================================================
 * Global variable definitions — each placed in a different ELF section
 * ==========================================================================
 *
 *       The IN_DATA / IN_BSS / IN_RODATA macros use __attribute__((section))
 *       to make the placement explicit and grep-able.  Without the attribute
 *       the compiler still places them correctly - the attribute just makes
 *       intent self-documenting and allows linker-script overrides.
 */

/* .rodata - linker will refuse any write attempt at runtime (SIGSEGV)      */
const uint32_t g_magic_number IN_RODATA = 0xDEADBEEFU;

/* .rodata - string literals are implicitly placed here                     */
static const char * const k_build_tag IN_RODATA = "v1.0.0-release";

/* .data - loaded from the ELF binary, consumes flash on embedded targets   */
uint32_t g_initialized_counter IN_DATA = 42U;

/* .data - local static with non-zero init                                  */
static uint32_t s_retry_limit IN_DATA = 3U;

/* .bss - no file space consumed; zero-filled by C runtime before main()   */
uint32_t g_zero_counter IN_BSS;

/* .bss - large buffer that would waste flash if stored in .data            */
uint8_t  g_rx_buffer[256] IN_BSS;

/* .bss - local static, zero-initialised                                    */
static uint32_t s_error_count IN_BSS;

/* ------------------------------------------------------------------------
 * Helper — print address and inferred section of a variable
 * ------------------------------------------------------------------------*/

/**
 * @brief  Print the runtime address of a variable alongside its expected
 *         section name.  The "expected section" is provided by the caller
 *         since we cannot query the section at runtime portably.
 *
 * On Linux you can verify with:
 *     nm --numeric-sort ./struct_demo | grep <symbol_name>
 *     objdump -t ./struct_demo | grep <symbol_name>
 */
static void print_addr(const char *name, const void *addr,
                       const char *section, const char *note)
{
    printf("  %-28s  %p  %-8s  %s\n", name, addr, section, note);
}

/* ------------------------------------------------------------------------
 * automatic (stack) variables
 * ------------------------------------------------------------------------*/

static void stack_variables(void)
{
    /*
     * Stack variables exist only for the duration of this function call.
     * Their addresses are relative to the stack pointer - NOT in any ELF
     * section.  Note that stack grows downward on x86-64.
     */
    uint8_t  stack_u8  = 0xAAU;
    uint32_t stack_u32 = 0xBBBBBBBBU;
    uint64_t stack_u64 = 0xCCCCCCCCCCCCCCCCULL;

    printf("\n  [Stack / automatic variables - NOT in any ELF section]\n");
    print_addr("stack_u8",  &stack_u8,  "stack", "automatic, 1 byte");
    print_addr("stack_u32", &stack_u32, "stack", "automatic, 4 bytes");
    print_addr("stack_u64", &stack_u64, "stack", "automatic, 8 bytes");

    printf("\n  Stack grows downward on x86-64:\n");
    printf("  &stack_u64 (%p) < &stack_u32 (%p) < &stack_u8 (%p)  ->  %s\n",
           (void *)&stack_u64, (void *)&stack_u32, (void *)&stack_u8,
           ((uintptr_t)&stack_u64 < (uintptr_t)&stack_u32) ? "confirmed" : "unexpected");

    /* suppress unused-variable warnings in release builds */
    (void)stack_u8; (void)stack_u32; (void)stack_u64;
}

/* ------------------------------------------------------------------------
 *  Public entry point
 * ------------------------------------------------------------------------*/

/**
 * @brief  Run the memory sections demo.
 *         Called from main().
 */
void memory_sections_run(void)
{
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf("                ELF MEMORY SECTIONS                             \n");
    printf("----------------------------------------------------------------\n");

    printf("\n  %-28s  %-18s  %-8s  %s\n",
           "Variable", "Address", "Section", "Notes");
    printf("  %-28s  %-18s  %-8s  %s\n",
           "----------------------------", "------------------",
           "--------", "-----");

    /* .rodata */
    print_addr("g_magic_number",      &g_magic_number,      ".rodata",
               "const, read-only, value=0xDEADBEEF");
    print_addr("k_build_tag (ptr)",   &k_build_tag,         ".rodata",
               "const char* const");
    print_addr("*k_build_tag (str)",  (const void *)k_build_tag, ".rodata",
               "string literal");

    /* .data */
    print_addr("g_initialized_counter", &g_initialized_counter, ".data",
               "non-const, init=42");
    print_addr("s_retry_limit",        &s_retry_limit,        ".data",
               "static, init=3");

    /* .bss */
    print_addr("g_zero_counter",      &g_zero_counter,      ".bss",
               "non-const, zero-init (no file space)");
    print_addr("g_rx_buffer[0]",      &g_rx_buffer[0],      ".bss",
               "256-byte array, zero-init");
    print_addr("s_error_count",       &s_error_count,       ".bss",
               "static, zero-init");

    /* Verify zero-initialisation of .bss */
    printf("\n  .bss zero-init verification:\n");
    printf("    g_zero_counter  = %u  (expected 0)\n", g_zero_counter);
    printf("    s_error_count   = %u  (expected 0)\n", s_error_count);
    printf("    g_rx_buffer[0]  = %u  (expected 0)\n", g_rx_buffer[0]);

    /* Demonstrate .data mutation */
    printf("\n  .data mutation demo:\n");
    printf("    g_initialized_counter before = %u\n", g_initialized_counter);
    g_initialized_counter++;
    printf("    g_initialized_counter after  = %u\n", g_initialized_counter);

    stack_variables();

    printf("\n  TIP: Run  'nm --numeric-sort ./struct_demo'  to confirm\n");
    printf("       section placement by examining the symbol table.\n");
    printf("\n  TIP: Run  'make sections'  to see raw ELF section sizes.\n");

    /* Suppress unused static warnings in some compilers */
    (void)s_retry_limit;
    (void)s_error_count;
}
