/**
 * @file CompassDriver.h
 * @author Felipe Telles, Lucas Martins
 * @brief For the HMC5883L magnetometer. Allows to get angle
 * relative to north
 * @date 27-11-2023
 *
 *
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#define _USE_MATH_DEFINES
#include <math.h>
#define RAD_TO_DEGREES(x) (((x) * 180) / M_PI)

/**
 * @brief Configure  configuration and mode registers.
 * Others register are set as default
 *
 * @return true ok
 * @return false not ok
 */
bool CompassDriver_Init();

/**
 * @brief Get the Angle relative to the north
 *
 * @return float in degrees
 */
bool CompassDriver_GetAngle(float* angle);
