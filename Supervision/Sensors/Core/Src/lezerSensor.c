/**
 * lezerSensor.c
 * 		Author: feket
 * 	Created on: Mar 16, 2021
 * 	All Rights Reserved.
 */

#include "lezerSensor.h"

volatile uint32_t Letimed = 0;
volatile uint32_t LeaTime;
volatile uint32_t LeoTime;
volatile uint32_t Ledistance = 0;
volatile uint8_t LeEdgeDetected = 0;

TIM_HandleTypeDef* Letim;

void initlezer(TIM_HandleTypeDef *htim){
	Letim = htim;
	HAL_TIM_IC_Start_IT(Letim, TIM_CHANNEL_3);

}

/*
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == Letim->Instance){
		if(LeEdgeDetected == 0){
			LeaTime = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
			LeEdgeDetected = 1;
		}
		else if (LeEdgeDetected == 1){
			LeoTime = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			if(LeaTime < LeoTime){
				Letimed = LeoTime - LeaTime;
			}
			else{
				Letimed = (0xffff - LeaTime) + LeoTime;
			}
			Ledistance=Letimed/10;

			LeEdgeDetected = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
		}
	}
}

*/
uint32_t getlezerDistance(void){
	return Ledistance;
}
