#include "Services/RudderControlDriver.h"

#include <Utils.h>
#include <stdbool.h>

#include "Services/Pwm3Driver.h"

#define MAX_ANGLE 90
#define MIN_ANGLE (-90)

#define MIN_PWM_VALUE     28
#define MAX_PWM_VALUE     127
#define NEUTRAL_PWM_VALUE 77

#define ANGLE_TO_PWM_INTERCEPT NEUTRAL_PWM_VALUE
#define ANGLE_TO_PWM_SLOPE                        \
    ((float)((NEUTRAL_PWM_VALUE)-MIN_PWM_VALUE) / \
     (float)MAX_ANGLE)
#define ANGLE_TO_PWM(x) \
    ((float)x * ANGLE_TO_PWM_SLOPE + ANGLE_TO_PWM_INTERCEPT)

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

    const int32_t pwmValue = ANGLE_TO_PWM(angle);

    Pwm3Driver_SetValue(PWM3_CHANNEL_RUDDER, pwmValue);
}
