#pragma once

#include "CMSIS_extra/cmsis_extra.h"
#include "Drivers/BLE_JDY_18.h"
#include "Drivers/HMC5883L.h"
#include "Drivers/PowerControlDriver.h"
#include "Drivers/RudderControlDriver.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"

#include <stdbool.h>
#include <stdint.h>

// PERIPHERALS

// QUEUES
extern osMessageQueueId_t q_rudderMessageHandle;
extern osMessageQueueId_t q_powerMessageHandle;
extern osMessageQueueId_t q_bleMessageHandle;
extern osMessageQueueId_t q_hmc5883LMessageHandle;

// TASKS
extern osThreadId_t navigationTaskHandle;
extern osThreadId_t powerMotorTaskHandle;
extern osThreadId_t rudderMotorTaskHandle;
extern osThreadId_t bleTaskHandle;
extern osThreadId_t Hmc5883LTaskHandle;

typedef struct {
    int32_t angle;
} rudderMessage_t;

typedef struct {
    int32_t percentage;
    bool isBackwards;
} powerMessage_t;

typedef struct {
    SlaveDevice_t slaves[3];
} bleMessage_t;

typedef struct {
    float angleFromNorth;
} hmc5883L_t;

// EVENT FLAGS

// SEMAPHORES

// MUTEXES

// TIMERS
