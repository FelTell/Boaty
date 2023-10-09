#include "Controllers/SystemController.h"

#include <stdbool.h>
#include <stdint.h>

#include "Services/PowerControlDriver.h"

#include "main.h"

static volatile uint32_t value = 0;

void SystemController_Init(void) {
    PowerControlDriver_Init();
}

void SystemController_Run(void) {}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case B1_Pin: {
            PowerControlDriver_SetPower(value += 10, false);
            if (value > 100) {
                value = 0;
            }
        } break;
    }
}
