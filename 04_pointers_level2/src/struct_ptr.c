/**
 * @file    struct_ptr.c
 * @brief   Struct pointer and dynamic memory management demonstrations.
 *
 *   CONCEPT: Pointer to Struct                                             
 *                                                                          
 *   Given:  SensorNode_t  node;                                            
 *           SensorNode_t *p = &node;                                       
 *                                                                          
 *   Access member via pointer:  p->temperature  (arrow operator)          
 *   Equivalent to:             (*p).temperature  (explicit deref)         
 *                                                                          
 *   Passing large structs by pointer avoids stack copying and is          
 *   essential in firmware where stack space is measured in kilobytes.     
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#include "struct_ptr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* --------------- Public Functions --------------------- */

void sensor_node_print(const SensorNode_t *node)
{
    /* Guard: never dereference a NULL pointer */
    if (node == NULL) {
        fprintf(stderr, "[ERROR] sensor_node_print: NULL pointer received.\n");
        return;
    }

    printf("   ----- SensorNode --------------------------------\n");
    printf("    ID          : %u\n",   node->sensor_id);
    printf("    Name        : %s\n",   node->name);
    printf("    Temperature : %.2f Celsuis\n", node->temperature);
    printf("    Status      : %s\n",   node->is_active ? "ACTIVE" : "FAULTED");
    printf("    Location    : (%.4f, %.4f)\n",
           node->location.latitude, node->location.longitude);
    printf("   -------------------------------------------------\n");
}

/* -------------------------------------------------------------------------- */

void sensor_node_update(SensorNode_t *node, float temperature, uint8_t is_active)
{
    if (node == NULL) {
        fprintf(stderr, "[ERROR] sensor_node_update: NULL pointer received.\n");
        return;
    }
    node->temperature = temperature;
    node->is_active   = is_active;
}

/* -------------------------------------------------------------------------- */

void struct_ptr_stack(void)
{
    printf("\n--------------------------------------------\n");
    printf(  "             Struct Pointer (Stack)         \n");
    printf(  "--------------------------------------------\n");

    /* Stack-allocated struct – no malloc needed for small, short-lived data */
    SensorNode_t node = {
        .sensor_id   = 101,
        .temperature = 23.5f,
        .is_active   = 1,
        .location    = { .latitude = 51.5074f, .longitude = -0.1278f }
    };
    /* strncpy is safer than strcpy - guarantees no buffer overrun */
    strncpy(node.name, "TempSensor_A", sizeof(node.name) - 1);
    node.name[sizeof(node.name) - 1] = '\0';

    /* Obtain a pointer to the stack struct */
    SensorNode_t *p_node = &node;

    printf("  Initial state (access via -> operator):\n");
    sensor_node_print(p_node);

    /* Mutate through pointer - changes are visible in the original struct */
    sensor_node_update(p_node, 37.8f, 0);
    printf("\n  After sensor_node_update (temp=37.8, active=0):\n");
    sensor_node_print(p_node);

    printf("\n  sizeof(SensorNode_t) = %zu bytes  "
           "(pointer avoids copying all of this onto the stack)\n",
           sizeof(SensorNode_t));
}

/* -------------------------------------------------------------------------- */

SensorNode_t *struct_ptr_heap_alloc(void)
{
    printf("\n--------------------------------------------\n");
    printf(  "         Struct Pointer (Heap/malloc)       \n");
    printf(  "--------------------------------------------\n");

    /* ------------------------------------------------------------------
     * malloc returns void* – always cast explicitly in C.
     * Always check for NULL before use. In real firmware, a NULL return
     * from malloc can indicate heap exhaustion - a critical fault.
     * ------------------------------------------------------------------ */
    SensorNode_t *sensor = (SensorNode_t *)malloc(sizeof(SensorNode_t));
    if (sensor == NULL) {
        fprintf(stderr, "[CRITICAL] Heap allocation failed for SensorNode_t.\n");
        return NULL;
    }

    /* Zero-init the struct to avoid reading undefined garbage */
    memset(sensor, 0, sizeof(SensorNode_t));

    /* Populate via the arrow operator */
    sensor->sensor_id       = 202;
    sensor->temperature     = -5.3f;
    sensor->is_active       = 1;
    sensor->location.latitude  = 48.8566f;   /* Paris, France */
    sensor->location.longitude =  2.3522f;
    strncpy(sensor->name, "CryoSensor_B", sizeof(sensor->name) - 1);
    sensor->name[sizeof(sensor->name) - 1] = '\0';

    printf("  Heap-allocated sensor at address: %p\n", (void *)sensor);
    sensor_node_print(sensor);

    /* ------------------------------------------------------------------
     * We return the pointer to the caller so they can use and free it.
     * This models the "create / use / destroy" lifecycle common in
     * embedded middleware (e.g., a device handle).
     * ------------------------------------------------------------------ */
    printf("\n  Returning heap pointer to caller for further use...\n");
    return sensor;
}
