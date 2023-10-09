#include "Services/PowerControlDriver.h"

#include <Utils.h>

#include "Services/Pwm3Driver.h"

static void SetDirection(bool isBackwards);

static bool initDone;

void PowerControlDriver_Init(void) {
    if (initDone) {
        return;
    }
    Pwm3Driver_Init();

    initDone = true;
}

void PowerControlDriver_SetPower(int32_t percentage,
                                 bool isBackwards) {
    if (!initDone) {
        return;
    }

    UTILS_CLAMP(percentage, 0, 100);

    SetDirection(isBackwards);
    Pwm3Driver_SetValue(PWM3_CHANNEL_POWER,
                        percentage * 10);
}

static void SetDirection(bool isBackwards) {
    if (isBackwards) {
        // set pins here
    } else {
        // set pins here
    }
}
