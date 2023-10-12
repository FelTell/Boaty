#pragma once

#include <stdbool.h>
#include <stdint.h>

void PowerControlDriver_Init(void);

void PowerControlDriver_SetPower(int32_t percentage,
                                 bool isBackwards);
