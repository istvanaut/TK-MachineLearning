#include "main.h"

//US Sensor BEGIN
extern uint32_t UStimeDifference;
extern uint32_t USStartTime;
extern uint32_t USStopTime;
extern uint32_t USdistance;
extern uint8_t USrisingEdgeDetected;

extern TIM_HandleTypeDef* UStim;
//US Sensor END


//Lezer Sensor BEGIN
extern uint32_t Letimed ;
extern uint32_t LeaTime;
extern uint32_t LeoTime;
extern uint32_t Ledistance;
extern uint8_t LeEdgeDetected;


extern TIM_HandleTypeDef* Letim;

//Lezer Sensor END


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

//US Sensor BEGIN
	if(htim->Instance == UStim->Instance && htim->Channel == TIM_CHANNEL_1){
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

		//US Sensor END

//Lezer BEGIN
	if(htim->Instance == Letim->Instance && htim->Channel == TIM_CHANNEL_3){
		if(LeEdgeDetected == 0){
			LeaTime = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
			LeEdgeDetected = 1;
		}
		else if (LeEdgeDetected == 1){
			LeoTime = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			if(LeaTime < LeoTime){
				Letimed = LeoTime - LeaTime;
			}
			else{
				Letimed = (0xffff - LeaTime) + LeoTime;
			}
			Ledistance=Letimed/10;

			LeEdgeDetected = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
		}
	}
	//Lezer END

}
