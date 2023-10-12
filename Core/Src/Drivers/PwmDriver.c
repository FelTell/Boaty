#include "Drivers/PwmDriver.h"

#include <stdbool.h>

#include "Utils.h"

#include "main.h"

#define ARR_MAX_VALUE 999
#define ARR_MIN_VALUE 0

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

static struct {
    TIM_HandleTypeDef* timInstance;
    uint32_t channel;
} pwm[PWM_OUTPUT_ENUM_SIZE] = {
    [PWM_OUTPUT_POWER]  = {.timInstance = &htim3,
                           .channel     = TIM_CHANNEL_2},
    [PWM_OUTPUT_RUDDER] = {.timInstance = &htim4,
                           .channel     = TIM_CHANNEL_1}};

static bool initDone;

void PwmDriver_Init(void) {
    if (initDone) {
        return;
    }

    for (uint8_t i = 0; i < PWM_OUTPUT_ENUM_SIZE; ++i) {
        HAL_TIM_PWM_Start_IT(pwm[i].timInstance,
                             pwm[i].channel);
    }

    initDone = true;
}

void PwmDriver_SetValue(pwm_outputs_e output,
                        int32_t value) {
    if (!initDone) {
        return;
    }

    UTILS_CLAMP(value, ARR_MIN_VALUE, ARR_MAX_VALUE);

    __HAL_TIM_SET_COMPARE(pwm[output].timInstance,
                          pwm[output].channel,
                          value);
}
