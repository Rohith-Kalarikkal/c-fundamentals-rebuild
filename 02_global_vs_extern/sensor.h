/**
 * @file    sensor.h
 * @brief   Public interface for the Sensor module.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

/* =========================================================================
 * PUBLIC TYPE DEFINITIONS
 * ========================================================================= */

/** Return codes used by sensor API functions. */
typedef enum
{
    SENSOR_OK    = 0,   /*  Operation successful.          */
    SENSOR_ERROR = 1,   /*  General sensor error.          */
    SENSOR_BUSY  = 2    /*  Sensor busy / not ready.       */
} SensorStatus_t;

/* =========================================================================
 * PUBLIC FUNCTION PROTOTYPES
 * ========================================================================= */

/**
 * @brief  Initialise the sensor hardware and internal state.
 * @return SENSOR_OK on success, SENSOR_ERROR otherwise.
 */
SensorStatus_t Sensor_Init(void);

/**
 * @brief  Read the current sensor value.
 * @param[out] p_value  Pointer to store the reading.
 * @return SENSOR_OK on success, SENSOR_ERROR if pointer is NULL or HW fault.
 */
SensorStatus_t Sensor_Read(uint32_t * const p_value);

/**
 * @brief  Return the total number of successful reads since init.
 *         Demonstrates accessing a module-private static global via an API
 *         (the preferred pattern over exposing the variable directly).
 * @return Read count.
 */
uint32_t Sensor_GetReadCount(void);

#endif /* SENSOR_H */
