#pragma once

#include <stdint.h>

typedef enum {
    PWM_OUTPUT_POWER,
    PWM_OUTPUT_RUDDER,

    PWM_OUTPUT_ENUM_SIZE
} pwm_outputs_e;

void PwmDriver_Init(void);

void PwmDriver_SetValue(pwm_outputs_e output,
                        int32_t value);
