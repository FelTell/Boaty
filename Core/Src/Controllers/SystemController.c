#include "Controllers/SystemController.h"

#include <stdbool.h>
#include <stdint.h>

#include "Drivers/MfsDriver.h"

#include "main.h"
#include "stm32f4xx_hal_tim.h"

extern TIM_HandleTypeDef htim2;

void SystemController_Init(void) {
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, true);

    HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_2);

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 500);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500);
}

void SystemController_Run(void) {}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case B1_Pin:
            // MfsDriver_ToggleLeds();
            break;
        // case SW1_Pin: {
        //     MfsDriver_SetLeds();
        //     break;
        // }
        case SW2_Pin: {
            MfsDriver_ClearLeds();
            break;
        }
        case SW3_Pin: {
            {
                static uint8_t i = 0xC0;
                MfsDriver_SendDisplayCode(i++, 0xF2);
                break;
            }
        }
    }
}

#define COUNTER_MAX 999
void HAL_TIM_PWM_PulseFinishedCallback(
    TIM_HandleTypeDef* htim) {
    static int16_t redValue;
    static int16_t greenValue;
    static int16_t blueValue;
    static bool isRedIncreasing   = true;
    static bool isGreenIncreasing = false;
    static bool isBlueIncreasing  = false;
    if (redValue >= COUNTER_MAX) {
        isRedIncreasing   = false;
        isGreenIncreasing = true;
        isBlueIncreasing  = false;
    } else if (greenValue >= COUNTER_MAX) {
        isRedIncreasing   = false;
        isGreenIncreasing = false;
        isBlueIncreasing  = true;
    } else if (blueValue >= COUNTER_MAX) {
        isRedIncreasing   = true;
        isGreenIncreasing = false;
        isBlueIncreasing  = false;
    }
    __HAL_TIM_SET_COMPARE(
        &htim2,
        TIM_CHANNEL_1,
        isRedIncreasing ? redValue++ : redValue--);
    __HAL_TIM_SET_COMPARE(
        &htim2,
        TIM_CHANNEL_3,
        isGreenIncreasing ? greenValue++ : greenValue--);
    __HAL_TIM_SET_COMPARE(
        &htim2,
        TIM_CHANNEL_2,
        isBlueIncreasing ? blueValue++ : blueValue--);
    if (redValue < 0) {
        redValue = 0;
    }
    if (greenValue < 0) {
        greenValue = 0;
    }
    if (blueValue < 0) {
        blueValue = 0;
    }
}
