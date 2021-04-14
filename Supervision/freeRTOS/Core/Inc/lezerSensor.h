/**
 * lezerSensor.h
 * 		Author: feket
 * 	Created on: Mar 16, 2021
 * 	All Rights Reserved.
 */

#include "main.h"
#include "cmsis_os.h"

void initlezer(TIM_HandleTypeDef *htim);



uint32_t getLaserDistance(void);

void setLaserDistanceCallback(uint32_t value);
