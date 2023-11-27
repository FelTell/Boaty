#include "timer_handler.h"

#include "stm32f4xx.h"

bool timer_wait_ms(uint32_t timer_start, uint32_t delay) {
    const uint32_t current_time = HAL_GetTick();
    if ((current_time - timer_start) >= delay) {
        return true;
    }
    return false;
}

void timer_restart(uint32_t* timer_to_restart) {
    *timer_to_restart = HAL_GetTick();
}
