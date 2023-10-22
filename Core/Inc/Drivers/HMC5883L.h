/*
 * HMC5883L.h
 *
 *  Created on: Oct 22, 2023
 *      Author:
 *
 */
#include "main.h"

#ifndef INC_DRIVERS_HMC5883L_H_
#define INC_DRIVERS_HMC5883L_H_

#define HMC5883L_I2C_ADDRESS 0x1E << 1
// Registradores
#define HMC5883L_CRA 0x00
#define HMC5883L_CRB 0x01
#define HMC5883L_MODE 0x02
#define HMC5883L_DATAX_MSB 0x03
#define HMC5883L_DATAX_LSB 0x04
#define HMC5883L_DATAY_MSB 0x05
#define HMC5883L_DATAY_LSB 0x06
#define HMC5883L_DATAZ_MSB 0x07
#define HMC5883L_DATAZ_LSB 0x08

// Config register A bits
#define HMC5883L_CRA_DO_BIT 7
#define HMC5883L_CRA_AVERAGE_BIT 6

// Config register B bits
#define HMC5883L_CRB_GAIN_BIT 7

// Mode bits
#define HMC5883L_MODE_CONTINOUS_BIT 0

void HMC5883L_Init(I2C_HandleTypeDef *hi2c1);
void HMC5883L_ReadData(I2C_HandleTypeDef *hi2c1, int16_t *x, int16_t *y, int16_t *z);

#endif /* INC_DRIVERS_HMC5883L_H_ */
