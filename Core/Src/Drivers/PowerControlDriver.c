#include "Drivers/PowerControlDriver.h"

#include <Utils.h>

#include "Drivers/PwmDriver.h"

#include "main.h"

static void SetDirection(bool isBackwards);
static void SetLatchPin(bool value);
static void SetClockPin(bool value);
static void SetDataPin(bool value);
static void SetDisablePin(bool value);

static bool initDone;

void PowerControlDriver_Init(void) {
    if (initDone) {
        return;
    }
    PwmDriver_Init();

    SetDataPin(false);
    SetDisablePin(false);
    SetLatchPin(false);
    SetClockPin(false);

    PowerControlDriver_SetPower(0, false);

    initDone = true;
}

void PowerControlDriver_SetPower(int32_t percentage,
                                 bool isBackwards) {
    if (!initDone) {
        return;
    }

    UTILS_CLAMP(percentage, 0, 100);

    SetDirection(isBackwards);
    PwmDriver_SetValue(PWM_OUTPUT_POWER, percentage * 10);
}

static void SetDirection(bool isBackwards) {
    const uint8_t directionRegisters =
        isBackwards ? 0b0000'1000 : 0b0000'0100;

    for (uint8_t i = 0; i < 8; ++i) {
        SetDataPin(directionRegisters & (1 << (7 - i)));

        SetClockPin(true);
        SetClockPin(false);
    }
    SetLatchPin(true);
    SetDataPin(false);
    SetLatchPin(false);
}

static void SetLatchPin(bool value) {
    HAL_GPIO_WritePin(
        DIR_LATCH_GPIO_Port,
        DIR_LATCH_Pin,
        value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
static void SetClockPin(bool value) {
    HAL_GPIO_WritePin(
        DIR_CLK_GPIO_Port,
        DIR_CLK_Pin,
        value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
static void SetDataPin(bool value) {
    HAL_GPIO_WritePin(
        DIR_SER_GPIO_Port,
        DIR_SER_Pin,
        value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
static void SetDisablePin(bool value) {
    HAL_GPIO_WritePin(
        DIR_EN_GPIO_Port,
        DIR_EN_Pin,
        value ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
