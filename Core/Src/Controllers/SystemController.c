#include "Controllers/SystemController.h"

#include <stdint.h>

#include "Drivers/MfsDriver.h"

#include "main.h"

void SystemController_Init(void) {}

void SystemController_Run(void) {}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case B1_Pin:
            // MfsDriver_ToggleLeds();
            break;
        case SW1_Pin: {
            MfsDriver_SetLeds();
            break;
        }
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
