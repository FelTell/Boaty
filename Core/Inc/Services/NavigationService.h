/**
 * @file NavigationService.h
 * @author Felipe Telles
 * @brief Handles everything related to the boat going to
 * the target, calculates position from the beacons location
 * and distance. Calculates the desired power and angle of
 * both motors
 * @date 08-11-2023
 *
 *
 */

#pragma once

void NavigationService_Init();
void NavigationService_Handler();
