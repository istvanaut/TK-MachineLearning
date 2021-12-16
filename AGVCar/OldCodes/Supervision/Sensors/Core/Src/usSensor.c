/**
 * usSensor.c
 * 		Author: Kolontari Peter
 * 	Created on: Mar 10, 2021
 * 	All Rights Reserved.
 */

#include "usSensor.h"

volatile uint32_t UStimeDifferenceLeft = 0;
volatile uint32_t USStartTimeLeft;
volatile uint32_t USStopTimeLeft;
volatile uint32_t USdistanceLeft = 0;
volatile uint8_t USrisingEdgeDetectedLeft = 0;

volatile uint32_t UStimeDifferenceRight = 0;
volatile uint32_t USStartTimeRight;
volatile uint32_t USStopTimeRight;
volatile uint32_t USdistanceRight = 0;
volatile uint8_t USrisingEdgeDetectedRight = 0;

TIM_HandleTypeDef* UStim;

void initUS(TIM_HandleTypeDef *htim){
	UStim = htim;
	//Start LEFT
	HAL_TIM_IC_Start_IT(UStim, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(UStim, TIM_CHANNEL_4);
	HAL_TIM_Base_Start_IT(UStim);
}

/*
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == UStim->Instance){
		if(USrisingEdgeDetected == 0){
			USStartTime = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
			USrisingEdgeDetected = 1;
		}
		else if (USrisingEdgeDetected == 1){
			USStopTime = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			if(USStartTime < USStopTime){
				UStimeDifference = USStopTime - USStartTime;
			}
			else{
				UStimeDifference = (0xffff - USStartTime) + USStopTime;
			}
			USdistance = UStimeDifference * 0.034 / 2.0;

			USrisingEdgeDetected = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
		}
	}
}
*/
uint32_t getUSTimeLeft(void){
	return UStimeDifferenceLeft;
}

uint32_t getUSDistanceLeft(void){
	return USdistanceLeft;
}

uint32_t getUSTimeRight(void){
	return UStimeDifferenceRight;
}

uint32_t getUSDistanceRight(void){
	return USdistanceRight;
}
