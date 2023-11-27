#include "Drivers/PowerControlDriver.h"

#include "Drivers/PwmDriver.h"
#include "Drivers/ShiftRegisterDriver.h"
#include "global_instances.h"

#include <Utils.h>

static void UpdateDirection();

static bool isCurrentDirectionBackwards = false;

void PowerControlDriver_Handler(void* argument) {
    (void)argument;
    PwmDriver_Init();
    ShiftRegisterDriver_Init();

    UpdateDirection();
    PowerControlDriver_SetPower(0, false);

    for (;;) {
        static powerMessage_t powerMessage;
        osMessageQueueGet(q_powerMessageHandle,
                          &powerMessage,
                          0,
                          osWaitForever);

        PowerControlDriver_SetPower(
            powerMessage.percentage,
            powerMessage.isBackwards);
    }
}

void PowerControlDriver_SetPower(int32_t percentage,
                                 bool isBackwards) {
    UTILS_CLAMP(percentage, 0, 100);
    if (isCurrentDirectionBackwards != isBackwards) {
        isCurrentDirectionBackwards = isBackwards;
        UpdateDirection();
    }

    PwmDriver_SetValue(PWM_OUTPUT_POWER, percentage * 10);
}

static void UpdateDirection(void) {
    ShiftRegisterDriver_UpdateRegisters(
        isCurrentDirectionBackwards ? 0x08 : 0x04);
}
