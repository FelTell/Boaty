#include "Services/RudderControlDriver.h"

#include <Utils.h>
#include <stdbool.h>

#include "Services/Pwm3Driver.h"

#define MAX_ANGLE 90
#define MIN_ANGLE (-90)

static bool initDone;

void RudderControlDriver_Init(void) {
    if (initDone) {
        return;
    }
    Pwm3Driver_Init();

    initDone = true;
}

void RudderControlDriver_SetAngle(int32_t angle) {
    if (!initDone) {
        return;
    }

    UTILS_CLAMP(angle, MIN_ANGLE, MAX_ANGLE);

    Pwm3Driver_SetValue(PWM3_CHANNEL_RUDDER, angle * 10);
}
