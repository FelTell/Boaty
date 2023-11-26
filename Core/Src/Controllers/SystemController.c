#include "Controllers/SystemController.h"

#include "Drivers/HMC5883L.h"
#include "Services/NavigationService.h"
#include "main.h"

#define DELAY_MS 1000

void SystemController_Init(void) {
    NavigationService_Init();
}

void SystemController_Run(void) {
    // TODO (Felipe): use "timer" to keep cycles with
    // defined period
    NavigationService_Handler();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case B1_Pin: {
        } break;
    }
}
