#include "Controllers/SystemController.h"

#include <stdbool.h>
#include <stdint.h>

#include "Services/PowerControlDriver.h"
#include "Services/RudderControlDriver.h"

#include "main.h"

static volatile uint32_t powerValue  = 0;
static volatile uint32_t rudderAngle = 0;

void SystemController_Init(void) {
    PowerControlDriver_Init();
    RudderControlDriver_Init();
}

void SystemController_Run(void) {
    RudderControlDriver_SetAngle(-90);
    HAL_Delay(1000);
    PowerControlDriver_SetPower(10,false);
    HAL_Delay(1000);
    RudderControlDriver_SetAngle(0);
    HAL_Delay(2000);
    RudderControlDriver_SetAngle(90);
    HAL_Delay(1000);
    PowerControlDriver_SetPower(80,false);
    HAL_Delay(1000);
    RudderControlDriver_SetAngle(0);
    HAL_Delay(2000);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case B1_Pin: {
            PowerControlDriver_SetPower(powerValue += 10,
                                        false);
            if (powerValue > 100) {
                powerValue = 0;
            }
            RudderControlDriver_SetAngle(rudderAngle += 10);
            if (rudderAngle > 90) {
                rudderAngle = -90;
            }
        } break;
    }
}
