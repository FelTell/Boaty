#include "Drivers/PowerControlDriver.h"

#include <Utils.h>

#include "Drivers/PwmDriver.h"
#include "Drivers/ShiftRegisterDriver.h"

static void UpdateDirection();

static bool initDone;
static bool isCurrentDirectionBackwards = false;

void PowerControlDriver_Init(void) {
    if (initDone) {
        return;
    }
    PwmDriver_Init();
    ShiftRegisterDriver_Init();

    UpdateDirection();
    PowerControlDriver_SetPower(0, false);

    initDone = true;
}

void PowerControlDriver_SetPower(int32_t percentage,
                                 bool isBackwards) {
    if (!initDone) {
        return;
    }

    UTILS_CLAMP(percentage, 0, 100);
    if (isCurrentDirectionBackwards != isBackwards) {
        isCurrentDirectionBackwards = isBackwards;
        UpdateDirection();
    }

    PwmDriver_SetValue(PWM_OUTPUT_POWER, percentage * 10);
}

static void UpdateDirection(void) {
    ShiftRegisterDriver_UpdateRegisters(
        isCurrentDirectionBackwards ? 0x08 : 0x04);
}
