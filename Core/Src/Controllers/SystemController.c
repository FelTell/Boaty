#include "Controllers/SystemController.h"

#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "stm32f4xx_hal_tim.h"

extern TIM_HandleTypeDef htim3;
static volatile  uint16_t pwmValue = 7;

void SystemController_Init(void) {
    HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwmValue);
}

void SystemController_Run(void) {}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case B1_Pin:
            __HAL_TIM_SET_COMPARE(&htim3,
                                  TIM_CHANNEL_2,
                                  pwmValue++);
            if (pwmValue > 99) {
                pwmValue = 7;
            }
            break;
    }
}
