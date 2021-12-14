/**
 * usSensor.h
 * 		Author: Kolontari Peter
 * 	Created on: Mar 10, 2021
 * 	All Rights Reserved.
 */

#include "main.h"

void initUS(TIM_HandleTypeDef *htim);

uint32_t getUSTimeLeft(void);

uint32_t getUSDistanceLeft(void);

uint32_t getUSTimeRight(void);

uint32_t getUSDistanceRight(void);
