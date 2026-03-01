/**
 * @file    actuator.h
 * @brief   Public interface for the Actuator module.
 *
 * @author  Rohith Kalarikkal Ramakrishnan
 * @version 1.0.0
 */

#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>
#include <stdbool.h>

/* =========================================================================
 * PUBLIC TYPE DEFINITIONS
 * ========================================================================= */

typedef enum
{
    ACTUATOR_OK    = 0,
    ACTUATOR_ERROR = 1
} ActuatorStatus_t;

/* =========================================================================
 * PUBLIC FUNCTION PROTOTYPES
 * ========================================================================= */

ActuatorStatus_t Actuator_Init(void);

/**
 * @brief  Drive the actuator to the requested level.
 * @param  level  Drive level 0-100 (%).
 * @return ACTUATOR_OK or ACTUATOR_ERROR.
 */
ActuatorStatus_t Actuator_SetLevel(uint8_t level);

/** @brief  Return whether the actuator is currently active. */
bool Actuator_IsActive(void);

#endif /* ACTUATOR_H */
