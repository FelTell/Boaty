#include "Drivers/ShiftRegisterDriver.h"

#include "main.h"

static void SetLatchPin(bool value);
static void SetClockPin(bool value);
static void SetDataPin(bool value);
static void SetDisablePin(bool value);

static bool initDone;

void ShiftRegisterDriver_Init(void) {
    if (initDone) {
        return;
    }

    SetDataPin(false);
    SetDisablePin(false);
    SetLatchPin(false);
    SetClockPin(false);

    initDone = true;
}

void ShiftRegisterDriver_UpdateRegisters(
    uint8_t registers) {
    for (uint8_t i = 0; i < 8; ++i) {
        SetDataPin(registers & (1 << (7 - i)));

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
