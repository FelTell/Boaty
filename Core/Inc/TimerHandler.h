/**
 * @file TimerHandler.h
 * @author Felipe Telles (felipe.melo.telles@gmail.com)
 * @brief Timer handler to allow a simple scheduler
 * @date 27-11-2023
 *
 *
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Waits a specific timer to be elapsed by a desired
 * amount. Allows multiple timers by sending a different
 * timer_start.
 *
 * @param timer_start Timer specifier.
 * @param delay Amount desired to wait, max is 2^32, which
 * is equal to approximately 50 days. Values higher than
 * that will not work as expected!
 * @return true The desired time has elapsed.
 * @return false The desired time has not been elapsed.
 */
bool Timer_WaitMs(uint32_t timer_start, uint32_t delay);
/**
 * @brief Update timer to current tick value.
 *
 * @return current time value
 */
uint32_t Timer_Update(void);
