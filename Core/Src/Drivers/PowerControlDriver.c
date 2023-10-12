#include "Drivers/PowerControlDriver.h"

#include <Utils.h>

#include "Drivers/PwmDriver.h"

static void SetDirection(bool isBackwards);

static bool initDone;

void PowerControlDriver_Init(void) {
    if (initDone) {
        return;
    }
    PwmDriver_Init();

    initDone = true;
}

void PowerControlDriver_SetPower(int32_t percentage,
                                 bool isBackwards) {
    if (!initDone) {
        return;
    }

    UTILS_CLAMP(percentage, 0, 100);

    SetDirection(isBackwards);
    PwmDriver_SetValue(PWM_OUTPUT_POWER, percentage * 10);
}

static void SetDirection(bool isBackwards) {
    if (isBackwards) {
        // set pins here
    } else {
        // set pins here
    }
}
