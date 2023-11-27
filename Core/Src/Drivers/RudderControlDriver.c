#include "Drivers/RudderControlDriver.h"

#include "Drivers/PwmDriver.h"
#include "global_instances.h"

#include <Utils.h>
#include <stdbool.h>

#define MAX_ANGLE 90
#define MIN_ANGLE (-90)

#define MIN_PWM_VALUE     28
#define MAX_PWM_VALUE     127
#define NEUTRAL_PWM_VALUE 77

#define ANGLE_TO_PWM_INTERCEPT NEUTRAL_PWM_VALUE
#define ANGLE_TO_PWM_SLOPE                                 \
    ((float)((NEUTRAL_PWM_VALUE)-MIN_PWM_VALUE) /          \
     (float)MAX_ANGLE)
#define ANGLE_TO_PWM(x)                                    \
    ((float)x * ANGLE_TO_PWM_SLOPE + ANGLE_TO_PWM_INTERCEPT)

void RudderControlDriver_Handler(void* argument) {
    (void)argument;

    PwmDriver_Init();

    RudderControlDriver_SetAngle(0);

    for (;;) {
        static rudderMessage_t rudderMessage;
        osMessageQueueGet(q_rudderMessageHandle,
                          &rudderMessage,
                          0,
                          osWaitForever);
        RudderControlDriver_SetAngle(rudderMessage.angle);
    }
}

void RudderControlDriver_SetAngle(int32_t angle) {
    UTILS_CLAMP(angle, MIN_ANGLE, MAX_ANGLE);

    const int32_t pwmValue = ANGLE_TO_PWM(angle);

    PwmDriver_SetValue(PWM_OUTPUT_RUDDER, pwmValue);
}
