#include "Drivers/BLE_JDY_18.h"

#include "TimerHandler.h"
#include <stdio.h>

#include "main.h"

extern UART_HandleTypeDef huart3;
#define UART_HANDLER huart3

#define MESSAGE_TIMEOUT_MS 10
#define MESSAGE_MAX_SIZE   100

#define COMMAND_SET_BAUD "AT+BAUD"
#define BAUD_SET_115200  "8"

#define COMMAND_SET_ROLE "AT+ROLE"
#define MODE_SET_MASTER  "1"

#define COMMAND_SCAN "AT+INQ"

static bool RequestScan();
static bool SendCommand(const char* command,
                        const char* parameter);

bool BeaconDistance_Init() {
    // Increase baudrate as default is too low (9600)
    if (!SendCommand(COMMAND_SET_BAUD, BAUD_SET_115200)) {
        return 0;
    }
    HAL_Delay(10);
    UART_HANDLER.Init.BaudRate = 115200;
    if (HAL_UART_Init(&UART_HANDLER) != HAL_OK) {
        return 0;
    }
    if (!SendCommand(COMMAND_SET_ROLE, MODE_SET_MASTER)) {
        return 0;
    }

    return 1;
}

bool BeaconDistance_Handler() {
    static uint32_t scanBeacontimer;

    if (Timer_WaitMs(scanBeacontimer, 200)) {
        scanBeacontimer = Timer_Update();
        if (!RequestScan()) {
            return 0;
        }
    }
    return 1;
}

static bool RequestScan() {
    if (!SendCommand(COMMAND_SCAN, "")) {
        return 0;
    }
    return 1;
}

static bool SendCommand(const char* command,
                        const char* parameter) {
    char message[MESSAGE_MAX_SIZE];

    // Comando AT completo
    const uint32_t size = snprintf(message,
                                   MESSAGE_MAX_SIZE,
                                   "%s%s\r\n",
                                   command,
                                   parameter);

    if (HAL_UART_Transmit(&UART_HANDLER,
                          (uint8_t*)message,
                          size,
                          MESSAGE_TIMEOUT_MS) != HAL_OK) {
        return 0;
    }
    return 1;
}
