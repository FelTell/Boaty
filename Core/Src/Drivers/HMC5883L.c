/*
 * HMC5883L.c
 *
 *  Created on: Oct 22, 2023
 *      Author:
 */

#include "Drivers/HMC5883L.h"



void HMC5883L_Init(I2C_HandleTypeDef *hi2c1) {
	  // Enable I2C1
	  HAL_I2C_Init(hi2c1);

	  // Write config reg A
	  uint8_t config_a = (1 << HMC5883L_CRA_DO_BIT) | (0x01 << HMC5883L_CRA_AVERAGE_BIT);
	  if(HAL_I2C_Master_Transmit(hi2c1, HMC5883L_I2C_ADDRESS, &config_a, 1, HAL_MAX_DELAY) != HAL_OK) Error_Handler();

	  // Write config reg B
	  uint8_t config_b = (1 << HMC5883L_CRB_GAIN_BIT);
	  if(HAL_I2C_Master_Transmit(hi2c1, HMC5883L_I2C_ADDRESS, &config_b, 1, HAL_MAX_DELAY) != HAL_OK) Error_Handler();
	  // Write mode reg
	  uint8_t mode = (1 << HMC5883L_MODE_CONTINOUS_BIT);
	  if(HAL_I2C_Master_Transmit(hi2c1, HMC5883L_I2C_ADDRESS, &mode, 1, HAL_MAX_DELAY) != HAL_OK) Error_Handler();
}

void HMC5883L_ReadData(I2C_HandleTypeDef *hi2c1, int16_t *x, int16_t *y, int16_t *z) {
	  // Read data reg
	  uint8_t data[6];
	  if(HAL_I2C_Master_Receive(hi2c1, HMC5883L_I2C_ADDRESS, data, 6, HAL_MAX_DELAY) != HAL_OK) Error_Handler();

	  *x = (int16_t)((data[0] << 8) | data[1]);
	  *y = (int16_t)((data[2] << 8) | data[3]);
	  *z = (int16_t)((data[4] << 8) | data[5]);
}
