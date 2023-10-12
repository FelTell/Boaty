#include "Controllers/SystemController.h"

#include "Drivers/PowerControlDriver.h"
#include "Drivers/RudderControlDriver.h"

#include "main.h"

#define DELAY_MS 1000

void SystemController_Init(void) {
    PowerControlDriver_Init();
    RudderControlDriver_Init();
}

void SystemController_Run(void) {
    PowerControlDriver_SetPower(40, false);
    RudderControlDriver_SetAngle(90);
    HAL_Delay(DELAY_MS);
    PowerControlDriver_SetPower(0, false);
    RudderControlDriver_SetAngle(0);
    HAL_Delay(DELAY_MS);
    PowerControlDriver_SetPower(40, true);
    RudderControlDriver_SetAngle(-90);
    HAL_Delay(DELAY_MS);
    PowerControlDriver_SetPower(0, false);
    RudderControlDriver_SetAngle(0);
    HAL_Delay(DELAY_MS);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case B1_Pin: {
        } break;
    }
}
