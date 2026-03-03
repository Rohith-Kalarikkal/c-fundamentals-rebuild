/**
 * @file    func_ptr.c
 * @brief   Function pointer demonstrations including a table-driven calculator.
 *
 *   CONCEPT: Function Pointer                                              
 *                                                                          
 *   A function pointer stores the address of a function.                  
 *                                                                          
 *   Declaration:   double (*fn)(double, double);                           
 *   Assignment:    fn = &my_add;   // or just fn = my_add;                
 *   Call:          result = fn(3.0, 4.0);                                 
 *                                                                          
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "func_ptr.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* ----------------- Arithmetic Operations ---------------------------- */

/** @brief Individual operation functions – each matches ArithmeticOp_t sig. */
static double op_add(double a, double b) { return a + b; }
static double op_sub(double a, double b) { return a - b; }
static double op_mul(double a, double b) { return a * b; }
static double op_div(double a, double b)
{
    if (b == 0.0) {
        fprintf(stderr, "[ERROR] Division by zero attempted.\n");
        return 0.0;
    }
    return a / b;
}

/* --------------------- Calculator Table --------------------------- */

/**
 * @brief  Lookup table mapping operator symbols to implementations.
 *
 *  Adding a new operation requires ONLY a new row in this table.
 *  No existing code needs to change – a clean Open/Closed design.
 *
 *  In ROM-constrained microcontrollers this table can be placed in flash
 *  using compiler-specific attributes (e.g. __attribute__((section(".rodata")))).
 */
static const CalcEntry_t CALC_TABLE[] = {
    { '+', "add",      op_add },
    { '-', "subtract", op_sub },
    { '*', "multiply", op_mul },
    { '/', "divide",   op_div },
};

/** Number of entries – computed at compile time, no magic numbers. */
#define CALC_TABLE_SIZE  (sizeof(CALC_TABLE) / sizeof(CALC_TABLE[0]))

/* ------------------ Event Callback Demo -------------------------- */

/** Simulated event codes (mirrors what a real HAL might define). */
#define EVENT_BUTTON_PRESS   (0x01U)
#define EVENT_SENSOR_READY   (0x02U)
#define EVENT_COMM_ERROR     (0x03U)

/**
 * @brief  Registered callback – invoked by the simulated "driver".
 *
 *  The void* user_data pattern lets the callback carry context without
 *  global variables, which is essential for reentrant driver designs.
 */
static void my_event_handler(uint32_t event_code, void *user_data)
{
    const char *tag = (user_data != NULL) ? (const char *)user_data : "N/A";

    printf("  [CALLBACK] Event=0x%02X  Context=\"%s\"  -> ", event_code, tag);
    switch (event_code) {
        case EVENT_BUTTON_PRESS: printf("Button pressed!\n");      break;
        case EVENT_SENSOR_READY: printf("Sensor data ready.\n");   break;
        case EVENT_COMM_ERROR:   printf("Communication error!\n"); break;
        default:                 printf("Unknown event.\n");       break;
    }
}

/**
 * @brief  Simulated peripheral driver that fires a callback.
 *
 *  In real firmware this would be called from an ISR or RTOS task.
 *  The callback pointer is passed in, making the driver reusable.
 */
static void simulated_driver_fire_event(EventCallback_t cb,
                                        uint32_t        event_code,
                                        void           *user_data)
{
    if (cb == NULL) {
        fprintf(stderr, "[ERROR] No callback registered event 0x%02X dropped.\n",
                event_code);
        return;
    }
    cb(event_code, user_data);   /* Invoke through the function pointer */
}

/* ---------------------- Struct with Function Pointer ------------------ */

/**
 * @brief  A minimal "class" – data + behaviour bundled in a struct.
 *         This is the C equivalent of a vtable / interface pattern.
 */
typedef struct {
    char   label[16];
    int    value;
    void (*print)(const char *label, int value);   /**< Method pointer */
    int  (*transform)(int value);                   /**< Strategy pointer */
} Widget_t;

/** Concrete print implementation A */
static void widget_print_verbose(const char *label, int value)
{
    printf("  [VERBOSE] Widget '%s' holds value: %d\n", label, value);
}

/** Concrete print implementation B */
static void widget_print_compact(const char *label, int value)
{
    printf("  %s=%d\n", label, value);
}

/** Two pluggable transform strategies */
static int transform_double(int v)  { return v * 2; }
static int transform_square(int v)  { return v * v; }

/* ----------------- Public Functions --------------------------- */

void func_ptr_basic(void)
{
    printf("\n ------------------------------------------\n");
    printf(  "          Function Pointer Basics          \n");
    printf(  " ------------------------------------------\n");

    /* Declare a function pointer variable of type ArithmeticOp_t */
    ArithmeticOp_t operation = NULL;

    /* Assign: point to op_add */
    operation = op_add;
    printf("  operation = op_add  ->  operation(10, 3) = %.0f\n",
           operation(10.0, 3.0));

    /* Reassign: point to op_mul without changing the call site */
    operation = op_mul;
    printf("  operation = op_mul  ->  operation(10, 3) = %.0f\n",
           operation(10.0, 3.0));

    /* Store function pointers in an array */
    ArithmeticOp_t ops[4] = { op_add, op_sub, op_mul, op_div };
    const char    *names[4] = { "add", "sub", "mul", "div" };
    printf("\n  Iterating over function pointer array (a=12, b=4):\n");
    for (int i = 0; i < 4; i++) {
        printf("    %-4s : %.2f\n", names[i], ops[i](12.0, 4.0));
    }
}

/* -------------------------------------------------------------------------- */

void calculator(double a, char op, double b)
{
    printf("\n -------------------------------------------\n");
    printf(  "          Function Ptr Calculator           \n");
    printf(  " -------------------------------------------\n");
    printf("  Expression: %.2f  %c  %.2f\n", a, op, b);

    /* Linear search through the lookup table */
    for (uint32_t i = 0; i < CALC_TABLE_SIZE; i++) {
        if (CALC_TABLE[i].symbol == op) {
            double result = CALC_TABLE[i].fn(a, b);   /* Call via fn ptr */
            printf("  Operation : %s\n", CALC_TABLE[i].name);
            printf("  Result    : %.4f\n", result);
            return;
        }
    }
    fprintf(stderr, "  [ERROR] Unknown operator '%c'.\n", op);
}

/* -------------------------------------------------------------------------- */

void func_ptr_in_struct(void)
{
    printf("\n--------------------------------------------\n");
    printf(  "         Function Pointer in Struct         \n");
    printf(  "--------------------------------------------\n");

    /* ---- Widget 1: verbose print + double strategy ---- */
    Widget_t w1 = {
        .value     = 5,
        .print     = widget_print_verbose,
        .transform = transform_double
    };
    strncpy(w1.label, "Counter", sizeof(w1.label) - 1);

    /* ---- Widget 2: compact print + square strategy ---- */
    Widget_t w2 = {
        .value     = 7,
        .print     = widget_print_compact,
        .transform = transform_square
    };
    strncpy(w2.label, "Sensor", sizeof(w2.label) - 1);

    Widget_t *widgets[] = { &w1, &w2 };
    uint32_t  count     = sizeof(widgets) / sizeof(widgets[0]);

    printf("  Before transform:\n");
    for (uint32_t i = 0; i < count; i++) {
        widgets[i]->print(widgets[i]->label, widgets[i]->value);
    }

    /* Apply each widget's own transform strategy through function pointer */
    for (uint32_t i = 0; i < count; i++) {
        widgets[i]->value = widgets[i]->transform(widgets[i]->value);
    }

    printf("\n  After transform (w1=double, w2=square):\n");
    for (uint32_t i = 0; i < count; i++) {
        widgets[i]->print(widgets[i]->label, widgets[i]->value);
    }

    /* Runtime swap of strategy – the same pointer infrastructure handles it */
    printf("\n  Swapping w1 strategy to 'square' at runtime:\n");
    w1.transform = transform_square;
    w1.value     = 3;
    w1.value     = w1.transform(w1.value);
    w1.print(w1.label, w1.value);
}

/* -------------------------------------------------------------------------- */

void event_callback(void)
{
    printf("\n--------------------------------------------\n");
    printf(  "           Event Callback Pattern           \n");
    printf(  "--------------------------------------------\n");

    /* Register our handler (in real code this goes into a driver struct) */
    EventCallback_t registered_cb = my_event_handler;
    const char     *context       = "MainApp";

    printf("  Firing simulated peripheral events:\n");
    simulated_driver_fire_event(registered_cb, EVENT_BUTTON_PRESS, (void *)context);
    simulated_driver_fire_event(registered_cb, EVENT_SENSOR_READY, (void *)context);
    simulated_driver_fire_event(registered_cb, EVENT_COMM_ERROR,   (void *)context);

    /* Test: no callback registered */
    printf("\n  Firing event with NULL callback (should log error):\n");
    simulated_driver_fire_event(NULL, EVENT_BUTTON_PRESS, NULL);
}
