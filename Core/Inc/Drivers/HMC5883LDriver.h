#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct HMC5883L_data {
    int16_t x_axis;
    int16_t y_axis;
    int16_t z_axis;
} calibration_offset_t;

bool HMC5883LDriver_Init();
bool HMC5883L_testConnection();

bool HMC5883LDriver_GetHeading(int16_t* x, int16_t* y,
                               int16_t* z);

// Calibration
calibration_offset_t HMC5883LDriver_Calibration();
