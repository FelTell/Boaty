#include "Drivers/BLE_JDT_18.h"

void BLE_setup(UART_HandleTypeDef *huartInterface, UART_HandleTypeDef *loggingInterface) {
	huart = huartInterface;
	loggingHuart = loggingInterface;
	sendInstruction(SET_PERMISSIONS, "11111");
}