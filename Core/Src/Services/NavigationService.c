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
#include "Drivers/CompassDriver.h"
#include "Drivers/PowerControlDriver.h"
#include "Drivers/RudderControlDriver.h"
#include "TimerHandler.h"
#include "Utils.h"
#include "main.h"

// TODO (Felipe): Estimate and define this values when the
// boat is ready and in our hands
#define BLE_POWER 0
#define POWER_KP  1
#define RUDDER_KP 1.5

#define BEACON_1_X -19.866425 // saída x
#define BEACON_1_Y -43.964556 // saída y
#define BEACON_2_X -19.866733 // chegada x
#define BEACON_2_Y -43.964666 // chegada y
#define BEACON_3_X -19.866581 // beacon x
#define BEACON_3_Y -43.964787 // beacon y

#define TARGET_BEACON_X BEACON_1_X
#define TARGET_BEACON_Y BEACON_1_Y

volatile float angleDebug;

/**
 * @brief Calculates the distance (radius) with the signal
 * strength of the beacon
 *
 * @param signalStrength RSSI value provided by the ble
 * module
 * @return float
 */
float GetDistanceFromBeacon(float signalStrength);

/**
 * @brief Trilaterate the position with the distance found
 * by the ble module. Since the beacons position are fixed
 * this is configured in a define above.
 *
 * @param distanceBeacon1
 * @param distanceBeacon2
 * @param distanceBeacon3
 * @param x
 * @param y
 */
void TrilateratePosition(float distanceBeacon1,
                         float distanceBeacon2,
                         float distanceBeacon3,
                         float* x,
                         float* y);

/**
 * @brief Calculates the desired angle with the boat
 * position and the target beacon position
 *
 * @param x
 * @param y
 * @return float
 */
float GetDesiredAngle(float x, float y);

/**
 * @brief Calculates the right power to send to the motor as
 * defined by the error of the angle. A smaller angle means
 * the motor will go full power to the target
 *
 * @param detected
 * @param desired
 * @return uint8_t
 */
uint8_t GetPowerPercentage(float detected, float desired);

/**
 * @brief Calculates the right rudder angle to reach the
 * target
 *
 * @param detected
 * @param desired
 * @return float
 */
float GetRudderAngle(float detected, float desired);

void NavigationService_Init() {
    PowerControlDriver_Init();
    RudderControlDriver_Init();

    // Small delay to allow sensors power up
    HAL_Delay(10);

    BeaconDistance_Init();
    // while (!CompassDriver_Init(true)) {
    // try to init until it is successful
    // }
}

void NavigationService_Handler() {
    static uint32_t calculatePositiontimer;

    BeaconDistance_Handler();

    // Run this function periodically
    if (!Timer_WaitMs(calculatePositiontimer, 1000)) {
        return;
    }
    calculatePositiontimer = Timer_Update();

    // BLE_scan_slaves_and_save(3);

    // float currentX;
    // float currentY;

    // TrilateratePosition(
    //     GetDistanceFromBeacon(slaves[1].signal_rssi),
    //     GetDistanceFromBeacon(slaves[2].signal_rssi),
    //     GetDistanceFromBeacon(slaves[3].signal_rssi),
    //     &currentX,
    //     &currentY);

    // float desiredAngle = 0;
    // GetDesiredAngle(currentX, currentY);

    // float detectedAngle;
    // if (!CompassDriver_GetAngle(&detectedAngle)) {
    //     // invalid value, so ignore this cycle
    //     return;
    // }
    // angleDebug = detectedAngle;
    // PowerControlDriver_SetPower(
    //     GetPowerPercentage(detectedAngle, desiredAngle),
    //     true);
    // RudderControlDriver_SetAngle(
    //     -GetRudderAngle(detectedAngle, desiredAngle));
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

float GetDesiredAngle(float x, float y) {
    const float deltaX = TARGET_BEACON_X - x;
    const float deltaY = TARGET_BEACON_Y - y;

    return RAD_TO_DEGREES(atan(deltaY / deltaX));
}

uint8_t GetPowerPercentage(float detected, float desired) {
    const float error = fabs(desired - detected);

    float power = (100 - (POWER_KP * error));

    UTILS_CLAMP(power, 75, 100);

    return power;
}
float GetRudderAngle(float detected, float desired) {
    const float error = desired - detected;
    return RUDDER_KP * error;
}
