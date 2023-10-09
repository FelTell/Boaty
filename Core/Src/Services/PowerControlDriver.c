#include "Services/PowerControlDriver.h"

#include <Utils.h>
#include <stdbool.h>

#include "Services/Pwm3Driver.h"

static bool initDone;

void PowerControlDriver_Init(void) {
    if (initDone) {
        return;
    }
    Pwm3Driver_Init();

    initDone = true;
}

void PowerControlDriver_SetPower(int32_t percentage) {
    if (!initDone) {
        return;
    }

    UTILS_CLAMP(percentage, 0, 100);

    Pwm3Driver_SetValue(PWM3_CHANNEL_POWER,
                        percentage * 10);
}
