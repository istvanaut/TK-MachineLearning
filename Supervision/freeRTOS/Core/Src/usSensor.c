/**
 * usSensor.c
 * 		Author: Kolontari Peter
 * 	Created on: Mar 10, 2021
 * 	All Rights Reserved.
 */

#include "usSensor.h"

enum US_SENSOR{
	LEFT, RIGHT
};

extern osSemaphoreId_t SemLeftUSSensorHandle;
extern osSemaphoreId_t SemRightUSSensorHandle;
extern osSemaphoreId_t SemUSSensorEdgeHandle;

volatile uint32_t UStimeDifferenceLeft = 0;
volatile uint32_t USStartTimeLeft;
volatile uint32_t USStopTimeLeft;
static volatile uint32_t USdistanceLeft = 0;
static volatile uint8_t USrisingEdgeDetectedLeft = 0;

volatile uint32_t UStimeDifferenceRight = 0;
volatile uint32_t USStartTimeRight;
volatile uint32_t USStopTimeRight;
static volatile uint32_t USdistanceRight = 0;
static volatile uint8_t USrisingEdgeDetectedRight = 0;

volatile enum US_SENSOR currentUSSensor = LEFT;

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

//Distance in cm
uint32_t getUSDistanceLeft(void){
	uint32_t temp;
	osSemaphoreAcquire(SemLeftUSSensorHandle, osWaitForever);
	temp = USdistanceLeft;
	osSemaphoreRelease(SemLeftUSSensorHandle);
	return temp;
}

//Distance in cm
uint32_t getUSDistanceRight(void){
	uint32_t temp;
	osSemaphoreAcquire(SemRightUSSensorHandle, osWaitForever);
	temp = USdistanceRight;
	osSemaphoreRelease(SemRightUSSensorHandle);
	return temp;
}

void setUSDistanceLeftCallBack(uint32_t value){
	if(osSemaphoreAcquire(SemLeftUSSensorHandle, 0) == osOK){
		USdistanceLeft = value;
		osSemaphoreRelease(SemLeftUSSensorHandle);
	}
	return;
}

void setUSDistanceRightCallBack(uint32_t value){
	if(osSemaphoreAcquire(SemRightUSSensorHandle, 0) == osOK){
		USdistanceRight = value;
		osSemaphoreRelease(SemRightUSSensorHandle);
	}
	return;
}

void setUSRisingEdgeLeftCallBack(uint8_t value){
	if(osSemaphoreAcquire(SemUSSensorEdgeHandle, 0) == osOK){
		USrisingEdgeDetectedLeft = value;
		osSemaphoreRelease(SemUSSensorEdgeHandle);
	}
	return;
}

void setUSRisingEdgeRightCallBack(uint8_t value){
	if(osSemaphoreAcquire(SemUSSensorEdgeHandle, 0) == osOK){
		USrisingEdgeDetectedRight = value;
		osSemaphoreRelease(SemUSSensorEdgeHandle);
	}
	return;
}

uint8_t getUSRisingEdgeLeftCallback(void){
	uint8_t temp = 0;
	if(osSemaphoreAcquire(SemUSSensorEdgeHandle, 0) == osOK){
		temp = USrisingEdgeDetectedLeft;
		osSemaphoreRelease(SemUSSensorEdgeHandle);
	}
	return temp;
}

uint8_t getUSRisingEdgeRightCallback(void){
	uint8_t temp = 0;
	if(osSemaphoreAcquire(SemUSSensorEdgeHandle, 0) == osOK){
		temp = USrisingEdgeDetectedRight;
		osSemaphoreRelease(SemUSSensorEdgeHandle);
	}
	return temp;
}

//Is should be called from HAL_TIM_IC_CaptureCallback
void USSensorInputCaptureCallback(TIM_HandleTypeDef *htim){
	//US Sensor Left BEGIN

	if(htim->Instance == UStim->Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3){
		if(getUSRisingEdgeLeftCallback() == 0){
			USStartTimeLeft = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
			setUSRisingEdgeLeftCallBack(1);
		}
		else if (getUSRisingEdgeLeftCallback() == 1){
			USStopTimeLeft = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			if(USStartTimeLeft < USStopTimeLeft){
				UStimeDifferenceLeft = USStopTimeLeft - USStartTimeLeft;
			}
			else{
				UStimeDifferenceLeft = (0xffff - USStartTimeLeft) + USStopTimeLeft;
			}
			setUSDistanceLeftCallBack(UStimeDifferenceLeft * 0.034 / 2.0);

			setUSRisingEdgeLeftCallBack(0);
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
		}
	}

	//US Sensor Left END
	//US Sensor Right BEGIN

	if(htim->Instance == UStim->Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
		if(getUSRisingEdgeRightCallback() == 0){
			USStartTimeRight = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
			setUSRisingEdgeRightCallBack(1);
		}
		else if (getUSRisingEdgeRightCallback() == 1){
			USStopTimeRight = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			if(USStartTimeRight < USStopTimeRight){
				UStimeDifferenceRight = USStopTimeRight - USStartTimeRight;
			}
			else{
				UStimeDifferenceRight = (0xffff - USStartTimeRight) + USStopTimeRight;
			}
			setUSDistanceRightCallBack(UStimeDifferenceRight * 0.034 / 2.0);

			setUSRisingEdgeRightCallBack(0);
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
		}
	}

	//US Sensor Right END
}

//It should be called from HAL_TIM_PeriodElapsedCallback
void USSensorPeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance != UStim->Instance)
		return;

	//LEFT
	if(currentUSSensor == LEFT){
		HAL_TIM_Base_Stop_IT(UStim);
		//Stop LEFT
		HAL_TIM_PWM_Stop(UStim, TIM_CHANNEL_4);
		HAL_TIM_IC_Stop_IT(UStim, TIM_CHANNEL_3);
		//If echo not detected, distance set to 500 cm
		if(getUSRisingEdgeLeftCallback() == 1) setUSDistanceLeftCallBack(500);
		setUSRisingEdgeLeftCallBack(0);
		//Set timer to zero
		__HAL_TIM_SET_COUNTER(UStim, 0);
		//Set RIGHT
		__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
		//Start RIGHT
		HAL_TIM_IC_Start_IT(UStim, TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(UStim, TIM_CHANNEL_2);
		HAL_TIM_Base_Start_IT(UStim);

		currentUSSensor = RIGHT;
	}
	//RIGHT
	else if(currentUSSensor == RIGHT){
		HAL_TIM_Base_Stop_IT(UStim);
		//Stop RIGHT
		HAL_TIM_PWM_Stop(UStim, TIM_CHANNEL_2);
		HAL_TIM_IC_Stop_IT(UStim, TIM_CHANNEL_1);
		//If echo not detected, distance set to 500 cm
		if(getUSRisingEdgeRightCallback() == 1) setUSDistanceRightCallBack(500);
		setUSRisingEdgeRightCallBack(0);
		//Set timer to zero
		__HAL_TIM_SET_COUNTER(UStim, 0);
		//Set LEFT
		__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
		//Start LEFT
		HAL_TIM_IC_Start_IT(UStim, TIM_CHANNEL_3);
		HAL_TIM_PWM_Start(UStim, TIM_CHANNEL_4);
		HAL_TIM_Base_Start_IT(UStim);

		currentUSSensor = LEFT;
	}
}
