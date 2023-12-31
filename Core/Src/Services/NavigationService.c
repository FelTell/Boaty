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

#define RUDDER_KP 1.5

// Position in coordinates, needs to be changed to another
// unit, preferably meters from start to match units
// calculated with the beacon rssi
#define BEACON_1_X -19.866425 // saída x
#define BEACON_1_Y -43.964556 // saída y
#define BEACON_2_X -19.866733 // chegada x
#define BEACON_2_Y -43.964666 // chegada y
#define BEACON_3_X -19.866581 // beacon x
#define BEACON_3_Y -43.964787 // beacon y

#define TARGET_BEACON_X BEACON_1_X
#define TARGET_BEACON_Y BEACON_1_Y

static uint32_t firstTimer;

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

    // while (!BeaconDistance_Init()) {
    //     // try to init until it is successful
    // }

    // NOTE (Felipe): this will take some time to calibrate
    // the compass. Where the boat is poiting at startup is
    // the north (0°)
    while (!CompassDriver_Init(true)) {
        // try to init until it is successful
    }
    firstTimer = Timer_Update();
}

void NavigationService_Handler() {
    static uint32_t calculatePositiontimer;

    // BeaconDistance_Handler();

    // Run this function periodically
    if (!Timer_WaitMs(calculatePositiontimer, 1000)) {
        return;
    }
    calculatePositiontimer = Timer_Update();

    float detectedAngle;
    if (!CompassDriver_GetAngle(&detectedAngle)) {
        return;
    }

    // NOTE (Felipe): The logic to use BLE beacons to get
    // the boat position proved to be unreliable. The
    // communication is working perfectly and we're
    // getting each beacon rssi values. But the rssi values
    // translate very badly to position. We were reading a
    // value of 5 meters for a beacon that was almost 40
    // meters away... Some amount of calibration fine tuning
    // and extreme filtering might allow a usable value, but
    // we decided it wasn't worthy and decided to focus on
    // compass navigation and a good calibration logic for
    // it

    // float beaconDistances[BEACONS_NUMBER];
    // float currentX;
    // float currentY;

    // if (!BeaconDistance_GetDistances(beaconDistances)) {
    //     // invalid value, so ignore this cycle
    //     return;
    // }

    // TrilateratePosition(beaconDistances[0],
    //                     beaconDistances[1],
    //                     beaconDistances[2],
    //                     &currentX,
    //                     &currentY);

    // float desiredAngle = 0;
    // GetDesiredAngle(currentX, currentY);

    // NOTE (Felipe): There is basicaly three stages of the
    // boat control

    // First: The power motor is off to allow to check if
    // compass and rudder is working as expected.

    // Second: The boat turn on the power motor and the
    // rudder will point forward

    // Third: The boat will point at the direction in which
    // it was turned on.
    if (Timer_WaitMs(firstTimer, 25000)) {
        PowerControlDriver_SetPower(100, true);
        RudderControlDriver_SetAngle(
            -GetRudderAngle(detectedAngle, 0));
    } else if (Timer_WaitMs(firstTimer, 10000)) {
        PowerControlDriver_SetPower(100, true);
        RudderControlDriver_SetAngle(0);
    } else {
        PowerControlDriver_SetPower(0, true);
        RudderControlDriver_SetAngle(
            -GetRudderAngle(detectedAngle, 0));
    }
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

float GetRudderAngle(float detected, float desired) {
    const float error = desired - detected;
    return RUDDER_KP * error;
}
