#include "Drivers/MfsDriver.h"

#include "main.h"
#include "stdbool.h"

void MfsDriver_ToggleLeds(void) {
    HAL_GPIO_TogglePin(D1_GPIO_Port, D1_Pin);
    HAL_GPIO_TogglePin(D2_GPIO_Port, D2_Pin);
    HAL_GPIO_TogglePin(D3_GPIO_Port, D3_Pin);
    HAL_GPIO_TogglePin(D4_GPIO_Port, D4_Pin);
}

void MfsDriver_SetLeds(void) {
    HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, 0);
    HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, 0);
    HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, 0);
    HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, 0);
}

void MfsDriver_ClearLeds(void) {
    HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, 1);
    HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, 1);
    HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, 1);
    HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, 1);
}

void MfsDriver_SendDisplayCode(uint8_t code,
                               uint8_t displayNum) {
    static uint16_t serial = 0;
    serial                 = code;
    serial <<= 8;
    serial |= displayNum;
    for (uint8_t i = 0; i < 16; i++) {
        if (serial & 0x8000) {
            HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, true);
        } else {
            HAL_GPIO_WritePin(SER_GPIO_Port,
                              SER_Pin,
                              false);
        }
        HAL_GPIO_WritePin(SRCLK_GPIO_Port,
                          SRCLK_GPIO_Port,
                          true);
        HAL_GPIO_WritePin(SRCLK_GPIO_Port,
                          SRCLK_GPIO_Port,
                          false);
        serial <<= 1;
    }
    HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_GPIO_Port, true);
    HAL_GPIO_WritePin(RCLK_GPIO_Port,
                      RCLK_GPIO_Port,
                      false);
}
