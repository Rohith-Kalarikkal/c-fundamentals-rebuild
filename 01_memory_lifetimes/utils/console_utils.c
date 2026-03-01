/**
 * @file    console_utils.c
 *
 * @brief   Console display utilities — banners, separators, summary table.
 *
 * @details Keeping all printf formatting here means business-logic modules
 *          (fsm.c, gpio_edge_counter.c, etc.) contain zero presentation code
 *          and can be unit-tested with a stub console layer if needed.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "console_utils.h"
#include "app_config.h"

/* ============================================================================
 * Public function implementations
 * ========================================================================== */

/**
 * @brief   Print the program banner to stdout.
 */
void Console_PrintBanner(void)
{
    printf("\n");
    printf("   --------------------------------------------------------------\n");
    printf("  |           LOCAL vs STATIC Variable Behavior - Demo           |\n");
    printf("  |          Embedded C  |  C11  |  Professional Reference       |\n");
    printf("   --------------------------------------------------------------\n");
}

/* -------------------------------------------------------------------------- */

/**
 * @brief   Print a titled section separator.
 *
 * @param   title   Null-terminated section label string.
 */
void Console_PrintSeparator(const char *title)
{
    /* Build separator string once at runtime; no VLA — fixed SEPARATOR_WIDTH */
    char line[SEPARATOR_WIDTH + 1U];
    memset(line, '=', SEPARATOR_WIDTH);
    line[SEPARATOR_WIDTH] = '\0';

    printf("\n%s\n  SECTION: %s\n%s\n", line, title, line);
}

/* -------------------------------------------------------------------------- */

/**
 * @brief   Print the properties summary table to stdout.
 */
void Console_PrintSummaryTable(void)
{
    printf("\n");
    printf("  %-22s  %-16s  %-16s\n", "Property",
           "Local (auto)", "Static Local");
    printf("  %-22s  %-16s  %-16s\n",
           "----------------------",
           "----------------",
           "----------------");
    printf("  %-22s  %-16s  %-16s\n", "Memory segment",
           "Stack",           "BSS / Data");
    printf("  %-22s  %-16s  %-16s\n", "Lifetime",
           "Function scope",  "Program lifetime");
    printf("  %-22s  %-16s  %-16s\n", "Initialization",
           "Every call",      "Once at startup");
    printf("  %-22s  %-16s  %-16s\n", "Value persistence",
           "No",              "Yes");
    printf("  %-22s  %-16s  %-16s\n", "Reentrant-safe",
           "Yes",             "No (shared)");
    printf("  %-22s  %-16s  %-16s\n", "Scope (visibility)",
           "Function only",   "Function only");
    printf("  %-22s  %-16s  %-16s\n", "Address stability",
           "Changes",         "Fixed");
    printf("  %-22s  %-16s  %-16s\n", "Safe to return ptr?",
           "NO (dangling!)",  "Yes (with care)");
    printf("\n");
}

/* End of file — console_utils.c */
