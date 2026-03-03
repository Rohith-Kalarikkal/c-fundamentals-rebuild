/**
 * @file    struct_ptr.h
 * @brief   Struct definition and pointer-to-struct demonstration interface.
 *
 * Covers:
 *   - Defining structs (plain and nested)
 *   - Accessing members via struct pointers (arrow operator ->)
 *   - Dynamic allocation / deallocation of struct objects
 *   - Passing structs by pointer to functions (efficient, no copy overhead)
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef STRUCT_PTR_H
#define STRUCT_PTR_H

#include <stdint.h>

/* --------------- Type Definitions ------------------------ */

/** @brief GPS coordinate (nested struct example). */
typedef struct {
    float latitude;
    float longitude;
} GpsCoord_t;

/**
 * @brief  Sensor node descriptor – typical embedded data model.
 *
 *  Using a typedef keeps the call sites readable and is consistent with
 *  MISRA-C / automotive coding standards.
 */
typedef struct {
    uint16_t   sensor_id;      /*  Unique hardware sensor ID           */
    char       name[32];       /*  Human-readable label                */
    float      temperature;    /*  Latest temperature reading (°C)     */
    uint8_t    is_active;      /*  1 = operational, 0 = faulted        */
    GpsCoord_t location;       /*  Physical installation coordinates   */
} SensorNode_t;

/* -------------------- Public API -------------------------- */

/**
 * @brief  Shows stack-allocated struct access via pointer (-> operator).
 */
void struct_ptr_stack(void);

/**
 * @brief  Dynamically allocates a SensorNode, populates it, prints it,
 *         then frees the memory.  Demonstrates malloc/free best practices.
 *
 * @return Pointer to heap-allocated SensorNode_t (caller must free), or NULL.
 */
SensorNode_t *struct_ptr_heap_alloc(void);

/**
 * @brief  Receives a const pointer to a struct – read-only, no copy overhead.
 *         Pattern used in firmware for passing config structs to drivers.
 *
 * @param  node  Pointer to the sensor node to display.
 */
void sensor_node_print(const SensorNode_t *node);

/**
 * @brief  Updates sensor readings via a non-const pointer.
 *
 * @param  node         Pointer to the sensor node to update.
 * @param  temperature  New temperature value.
 * @param  is_active    New active state.
 */
void sensor_node_update(SensorNode_t *node, float temperature, uint8_t is_active);

#endif /* STRUCT_PTR_H */
