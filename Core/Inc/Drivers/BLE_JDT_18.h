#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "main.h"

void BLE_setup(UART_HandleTypeDef *huartInterface, UART_HandleTypeDef *loggingInterface);
