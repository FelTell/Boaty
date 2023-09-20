#pragma once

#include <stdint.h>

void MfsDriver_ToggleLeds(void);
void MfsDriver_SetLeds(void);
void MfsDriver_ClearLeds(void);
void MfsDriver_SendDisplayCode(uint8_t code,
                               uint8_t displayNum);
