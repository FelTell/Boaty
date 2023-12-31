#pragma once

#include <stdbool.h>
#include <stdint.h>

#define BEACONS_NUMBER 3

bool BeaconDistance_Init(void);
bool BeaconDistance_Handler(void);
bool BeaconDistance_GetDistances(
    float pDistances[BEACONS_NUMBER]);
void BeaconDistance_ErrorCallback(void);
void BeaconDistance_IdleCallback(void);
