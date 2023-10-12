#include "Services/RudderControlDriver.h"

#include <Utils.h>
#include <stdbool.h>

#include "Services/Pwm3Driver.h"

#define MAX_ANGLE 90
#define MIN_ANGLE (-90)

#define MIN_PWM_VALUE     35
#define MAX_PWM_VALUE     115
#define NEUTRAL_PWM_VALUE 75

#define ANGLE_TO_PWM_INTERCEPT 75
#define ANGLE_TO_PWM_SLOPE     (4.f / 9.f)

static bool initDone;

void RudderControlDriver_Init(void) {
    if (initDone) {
        return;
    }
    Pwm3Driver_Init();

    RudderControlDriver_SetAngle(0);

    initDone = true;
}

void RudderControlDriver_SetAngle(int32_t angle) {
    if (!initDone) {
        return;
    }

    UTILS_CLAMP(angle, MIN_ANGLE, MAX_ANGLE);

    const int32_t pwmValue =
        (int32_t)((float)angle * ANGLE_TO_PWM_SLOPE +
                  ANGLE_TO_PWM_INTERCEPT);

    Pwm3Driver_SetValue(PWM3_CHANNEL_RUDDER, pwmValue);
}
