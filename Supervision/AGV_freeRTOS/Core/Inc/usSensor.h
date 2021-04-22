/**
 * usSensor.h
 * 		Author: Kolontari Peter
 * 	Created on: Mar 10, 2021
 * 	All Rights Reserved.
 */

#include "main.h"
#include "cmsis_os.h"

void initUS(TIM_HandleTypeDef *htim);

uint32_t getUSDistanceLeft(void);

uint32_t getUSDistanceRight(void);

void setUSDistanceLeftCallBack(uint32_t value);

void setUSDistanceRightCallBack(uint32_t value);

void setUSRisingEdgeLeftCallBack(uint8_t value);

void setUSRisingEdgeRightCallBack(uint8_t value);

uint8_t getUSRisingEdgeLeftCallback(void);

uint8_t getUSRisingEdgeRightCallback(void);
