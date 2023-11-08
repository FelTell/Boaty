#include "Controllers/SystemController.h"

#include "Drivers/HMC5883L.h"
#include "Drivers/PowerControlDriver.h"
#include "Drivers/RudderControlDriver.h"
#include "main.h"

#define DELAY_MS 1000

int16_t x, y, z;
calibration_offset_t HMC_offset;

void SystemController_Init(void) {
    PowerControlDriver_Init();
    RudderControlDriver_Init();
    HMC5883LDriver_Init();
    HMC_offset = HMC5883LDriver_Calibration();
}

void SystemController_Run(void) {
    HMC5883LDriver_GetHeading(&x, &y, &z);
    // actual HMC5883L value
    x -= HMC_offset.x_axis;
    y -= HMC_offset.y_axis;
    z -= HMC_offset.z_axis;
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    HAL_Delay(50);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case B1_Pin: {
        } break;
    }
}
