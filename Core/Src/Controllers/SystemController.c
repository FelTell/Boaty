#include "Controllers/SystemController.h"

#include "main.h"

#include <stdint.h>

void SystemController_Init(void) {}

void SystemController_Run(void) {}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == B1_Pin) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    }
}
