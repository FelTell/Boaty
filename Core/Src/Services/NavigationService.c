/**
 * @file NavigationService.h
 * @author Felipe Telles
 * @brief Handles everything related to the boat going to
 * the target, calculates position from the beacons location
 * and distance. Calculates the desired power and angle of
 * both motors
 * @date 08-11-2023
 *
 *
 */

#include "Services/NavigationService.h"

#include "Drivers/BLE_JDY_18.h"
#include "Drivers/HMC5883L.h"
#include "Drivers/PowerControlDriver.h"
#include "Drivers/RudderControlDriver.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define RAD_TO_DEGREES(x) (((x)*180) / M_PI)

// TODO (Felipe): Estimate and define this values when the
// boat is ready and in our hands
#define BLE_POWER 0
#define POWER_KP  0
#define RUDDER_KP 0

#define BEACON_1_X 0
#define BEACON_1_Y 0
#define BEACON_2_X 0
#define BEACON_2_Y 0
#define BEACON_3_X 0
#define BEACON_3_Y 0

#define TARGET_BEACON_X BEACON_1_X
#define TARGET_BEACON_Y BEACON_1_Y

float GetDistanceFromBeacon(float signalStrength);
void TrilateratePosition(float distanceBeacon1,
                         float distanceBeacon2,
                         float distanceBeacon3,
                         float* x,
                         float* y);
float GetAngleFromNorth();
float GetDesiredAngle(float x, float y);
uint8_t GetPowerPercentage(float detected, float desired);
float GetRudderAngle(float detected, float desired);

static calibration_offset_t HMC_offset;

void NavigationService_Init() {
    PowerControlDriver_Init();
    RudderControlDriver_Init();
    HMC5883LDriver_Init();

    HMC_offset = HMC5883LDriver_Calibration();
}

void NavigationService_Handler() {
    SlaveDevice_t slaves[3];
    BLE_scan_slaves_and_save(slaves, 3);

    float currentX;
    float currentY;

    TrilateratePosition(
        GetDistanceFromBeacon(slaves[1].signal_rssi),
        GetDistanceFromBeacon(slaves[2].signal_rssi),
        GetDistanceFromBeacon(slaves[3].signal_rssi),
        &currentX,
        &currentY);

    float desiredAngle =
        GetDesiredAngle(currentX, currentY);
    float detectedAngle = GetAngleFromNorth();
    PowerControlDriver_SetPower(
        GetPowerPercentage(detectedAngle, desiredAngle),
        false);
    RudderControlDriver_SetAngle(
        GetRudderAngle(detectedAngle, desiredAngle));
}

float GetDistanceFromBeacon(float signalStrength) {
    return pow(10, (BLE_POWER - signalStrength) / (2 * 10));
}

void TrilateratePosition(float distanceBeacon1,
                         float distanceBeacon2,
                         float distanceBeacon3,
                         float* x,
                         float* y) {
    const float a1 = 2 * (BEACON_2_X - BEACON_1_X);
    const float b1 = 2 * (BEACON_2_Y - BEACON_1_Y);
    const float c1 =
        BEACON_1_X * BEACON_1_X + BEACON_1_Y * BEACON_1_Y -
        BEACON_2_X * BEACON_2_X - BEACON_2_Y * BEACON_2_Y -
        distanceBeacon1 * distanceBeacon1 +
        distanceBeacon2 * distanceBeacon2;

    const float a2 = 2 * (BEACON_3_X - BEACON_1_X);
    const float b2 = 2 * (BEACON_3_Y - BEACON_1_Y);
    const float c2 =
        BEACON_1_X * BEACON_1_X + BEACON_1_Y * BEACON_1_Y -
        BEACON_3_X * BEACON_3_X - BEACON_3_Y * BEACON_3_Y -
        distanceBeacon1 * distanceBeacon1 +
        distanceBeacon3 * distanceBeacon3;

    *x = (c1 * b2 - c2 * b1) / (a1 * b2 - a2 * b1);
    *y = (a1 * c2 - a2 * c1) / (a1 * b2 - a2 * b1);
}

float GetAngleFromNorth() {
    int16_t compassX;
    int16_t compassY;
    int16_t compassZ;
    HMC5883LDriver_GetHeading(
        &compassX, &compassY, &compassZ);
    // actual HMC5883L value
    compassX -= HMC_offset.x_axis;
    compassY -= HMC_offset.y_axis;
    compassZ -= HMC_offset.z_axis;

    return RAD_TO_DEGREES(atanf(compassX / compassY));
}

float GetDesiredAngle(float x, float y) {
    const float deltaX = TARGET_BEACON_X - x;
    const float deltaY = TARGET_BEACON_Y - y;

    return RAD_TO_DEGREES(atan(deltaY / deltaX));
}

uint8_t GetPowerPercentage(float detected, float desired) {
    const float error = fabs(desired - detected);
    return 100 - (POWER_KP * error);
}
float GetRudderAngle(float detected, float desired) {
    const float error = desired - detected;
    return RUDDER_KP * error;
}
