#include "main.h"

//US Sensor BEGIN
extern uint32_t UStimeDifference;
extern uint32_t USStartTime;
extern uint32_t USStopTime;
extern uint32_t USdistance;
extern uint8_t USrisingEdgeDetected;

extern TIM_HandleTypeDef* UStim;
//US Sensor END

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

//US Sensor BEGIN
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
//US Sensor END
}
