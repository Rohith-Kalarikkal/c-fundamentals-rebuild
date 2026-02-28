/**
 * @file    console_utils.h
 *
 * @brief   Public API for console display utility functions.
 *
 * @details Thin presentation layer — keeps all printf formatting logic out
 *          of business-logic modules so they remain unit-testable without
 *          stdout dependencies.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief   Print the program banner to stdout.
 */
void Console_PrintBanner(void);

/**
 * @brief   Print a titled section separator to stdout.
 *
 * @param   title   Null-terminated section title string.
 */
void Console_PrintSeparator(const char *title);

/**
 * @brief   Print the summary comparison table to stdout.
 */
void Console_PrintSummaryTable(void);

#endif /* CONSOLE_UTILS_H */

/* End of file — console_utils.h */
