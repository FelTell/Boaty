#include "timer_handler.h"

#include "stm32f4xx.h"

bool timer_wait_ms(uint32_t timer_start, uint32_t delay) {
    const uint32_t current_time = HAL_GetTick();
    if ((current_time - timer_start) >= delay) {
        return true;
    }
    return false;
}

uint32_t timer_update(void) {
    return HAL_GetTick();
}
