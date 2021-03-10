/**
 * usSensor.c
 * 		Author: Kolontari Peter
 * 	Created on: Mar 10, 2021
 * 	All Rights Reserved.
 */

#include "usSensor.h"

volatile uint32_t timeDifference = 0;
volatile uint32_t StartTime;
volatile uint32_t StopTime;
volatile uint32_t USdistance = 0;
volatile uint8_t risingEdgeDetected = 0;

TIM_HandleTypeDef* UStim;

void initUS(TIM_HandleTypeDef *htim){
	UStim = htim;
	HAL_TIM_IC_Start_IT(UStim, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(UStim, TIM_CHANNEL_2);
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == UStim->Instance){
		if(risingEdgeDetected == 0){
			StartTime = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
			risingEdgeDetected = 1;
		}
		else if (risingEdgeDetected == 1){
			StopTime = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			if(StartTime < StopTime){
				timeDifference = StopTime - StartTime;
			}
			else{
				timeDifference = (0xffff - StartTime) + StopTime;
			}
			USdistance = timeDifference * 0.034 / 2.0;

			risingEdgeDetected = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
		}
	}
}

uint32_t getUSTime(void){
	return timeDifference;
}

uint32_t getUSDistance(void){
	return USdistance;
}
