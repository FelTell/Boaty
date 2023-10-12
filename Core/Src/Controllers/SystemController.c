#include "Controllers/SystemController.h"

#include "Drivers/PowerControlDriver.h"
#include "Drivers/RudderControlDriver.h"

#include "main.h"

#define DELAY_MS 4000

void SystemController_Init(void) {
    PowerControlDriver_Init();
    RudderControlDriver_Init();
}

void SystemController_Run(void) {
    PowerControlDriver_SetPower(40, false);
    HAL_Delay(DELAY_MS);
    PowerControlDriver_SetPower(40, true);
    HAL_Delay(DELAY_MS);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case B1_Pin: {
        } break;
    }
}
