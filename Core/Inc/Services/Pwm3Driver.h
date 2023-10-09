#pragma once

#include <stdint.h>

typedef enum {
    PWM3_CHANNEL_POWER,
    PWM3_CHANNEL_RUDDER,
} pwm3_channels_e;

void Pwm3Driver_Init(void);

void Pwm3Driver_SetValue(pwm3_channels_e channel,
                         int32_t value);
