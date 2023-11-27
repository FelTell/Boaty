#pragma once

#include <stdbool.h>
#include <stdint.h>

void PowerControlDriver_Handler(void *argument );

void PowerControlDriver_SetPower(int32_t percentage,
                                 bool isBackwards);
