/**
 * @file    func_ptr.h
 * @brief   Function pointer demonstration interface.
 *
 * Covers:
 *   - Declaring and calling function pointers
 *   - Storing function pointers in structs (virtual-dispatch pattern)
 *   - A lookup-table-based arithmetic calculator
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef FUNC_PTR_H
#define FUNC_PTR_H

#include <stdint.h>

/* -------------------- Type Definitions --------------------------------- */

/**
 * @brief  Generic binary arithmetic operation: (double, double) -> double.
 *
 *  Defining a typedef for function pointers is mandatory in professional
 *  codebases – it makes declarations readable and reduces copy-paste errors.
 */
typedef double (*ArithmeticOp_t)(double a, double b);

/**
 * @brief  Calculator entry – pairs an operator symbol with its implementation.
 *
 *  This table-driven approach avoids long if/else or switch chains and
 *  makes adding new operations a one-line change (Open/Closed Principle).
 */
typedef struct {
    char           symbol;   /*  Single-char operator: '+', '-', '*', '/'  */
    const char    *name;     /*  Human-readable name for logging            */
    ArithmeticOp_t fn;       /*  Pointer to the operation implementation    */
} CalcEntry_t;

/**
 * @brief  Event callback type – common in embedded HAL / RTOS designs.
 *         A peripheral driver calls this when an event occurs.
 */
typedef void (*EventCallback_t)(uint32_t event_code, void *user_data);

/* -------------------- Public API ---------------------------- */

/** @brief Basic function pointer declaration and direct call demo. */
void func_ptr_basic(void);

/**
 * @brief  Runs the lookup-table calculator.
 *
 * @param  a   Left operand
 * @param  op  Operator character ('+', '-', '*', '/')
 * @param  b   Right operand
 */
void calculator(double a, char op, double b);

/**
 * @brief  Demonstrates storing a function pointer inside a struct and
 *         invoking it via a struct pointer (poor-man's polymorphism).
 */
void func_ptr_in_struct(void);

/**
 * @brief  Simulates registering and firing an event callback –
 *         pattern used in GPIO IRQ handlers, UART drivers, etc.
 */
void event_callback(void);

#endif /* FUNC_PTR_H */
