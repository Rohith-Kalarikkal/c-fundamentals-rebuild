/**
 * @file    include/memory_sections.h
 * @brief   Declarations for the .text / .data / .bss / .rodata section.
 *
 * ELF Section Primer
 * ------------------
 * .text   – Read-only, executable.  Contains compiled machine code and
 *           string literals on some toolchains.
 * .rodata – Read-only, non-executable.  const globals, string literals.
 * .data   – Read-write, initialised.  Global / static variables with a
 *           non-zero initial value.  Loaded from the ELF image at startup.
 * .bss    – Read-write, zero-initialised.  Global / static variables with
 *           no initialiser (or explicit = 0).  Takes NO space in the ELF
 *           file; the OS/startup code zero-fills at runtime.
 *
 * Stack   – Automatic (local) variables.  Not an ELF section; managed by
 *           the processor stack pointer at runtime.
 * Heap    – malloc'd storage.  Also not an ELF section.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef INCLUDE_MEMORY_SECTIONS_H
#define INCLUDE_MEMORY_SECTIONS_H

#include <stdint.h>
#include "alignment.h"   /* IN_DATA, IN_BSS, IN_RODATA macros                */

/* --- Variables whose addresses prove their section placement --------------*/

/* .rodata - read-only constant, placed by the compiler in .rodata           */
extern const uint32_t g_magic_number;

/* .data - initialised with a non-zero value -> lives in .data               */
extern uint32_t g_initialized_counter;

/* .bss - zero-initialised (no explicit initialiser) -> lives in .bss        */
extern uint32_t g_zero_counter;

/* .bss - explicitly zeroed global -> also .bss (no file storage needed)     */
extern uint8_t  g_rx_buffer[256];

/* --- Public function declarations ---------------------------------------- */
void memory_sections_run(void);

#endif /* INCLUDE_MEMORY_SECTIONS_H */
