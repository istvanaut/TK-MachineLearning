/**
 * usSensor.h
 * 		Author: Kolontari Peter
 * 	Created on: Mar 10, 2021
 * 	All Rights Reserved.
 */

#include "main.h"

void initUS(TIM_HandleTypeDef *htim);

uint32_t getTime(void);

uint32_t getUSDistance(void);
