#include "Services/Pwm3Driver.h"

#include <stdbool.h>

#include "Utils.h"

#include "main.h"

#define ARR_MAX_VALUE (999)
#define ARR_MIN_VALUE (0)

extern TIM_HandleTypeDef htim3;

static bool initDone;

void Pwm3Driver_Init(void) {
    if (initDone) {
        return;
    }

    HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);

    Pwm3Driver_SetValue(PWM3_CHANNEL_POWER, 0);
    Pwm3Driver_SetValue(PWM3_CHANNEL_RUDDER, 0);

    initDone = true;
}

void Pwm3Driver_SetValue(pwm3_channels_e channel,
                         int32_t value) {
    if (!initDone) {
        return;
    }

    UTILS_CLAMP(value, ARR_MIN_VALUE, ARR_MAX_VALUE);

    uint32_t HAL_CHANNEL;
    switch (channel) {
        case PWM3_CHANNEL_POWER:
            HAL_CHANNEL = TIM_CHANNEL_1;
            break;
        case PWM3_CHANNEL_RUDDER:
            HAL_CHANNEL = TIM_CHANNEL_2;
            break;
        default:
            return;
    }
    __HAL_TIM_SET_COMPARE(&htim3, HAL_CHANNEL, value);
}
