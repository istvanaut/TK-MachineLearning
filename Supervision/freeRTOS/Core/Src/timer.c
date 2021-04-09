#include "main.h"

//US Sensor BEGIN
extern uint32_t UStimeDifferenceLeft;
extern uint32_t USStartTimeLeft;
extern uint32_t USStopTimeLeft;
extern uint32_t USdistanceLeft;
extern uint8_t USrisingEdgeDetectedLeft;

extern uint32_t UStimeDifferenceRight;
extern uint32_t USStartTimeRight;
extern uint32_t USStopTimeRight;
extern uint32_t USdistanceRight;
extern uint8_t USrisingEdgeDetectedRight;

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

//US Sensor Left BEGIN

	if(htim->Instance == UStim->Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3){
		if(USrisingEdgeDetectedLeft == 0){
			USStartTimeLeft = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_FALLING);
			USrisingEdgeDetectedLeft = 1;
		}
		else if (USrisingEdgeDetectedLeft == 1){
			USStopTimeLeft = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			if(USStartTimeLeft < USStopTimeLeft){
				UStimeDifferenceLeft = USStopTimeLeft - USStartTimeLeft;
			}
			else{
				UStimeDifferenceLeft = (0xffff - USStartTimeLeft) + USStopTimeLeft;
			}
			USdistanceLeft = UStimeDifferenceLeft * 0.034 / 2.0;

			USrisingEdgeDetectedLeft = 0;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
		}
	}

//US Sensor Left END
//US Sensor Right BEGIN

	if(htim->Instance == UStim->Instance && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
			if(USrisingEdgeDetectedRight == 0){
				USStartTimeRight = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
				USrisingEdgeDetectedRight = 1;
			}
			else if (USrisingEdgeDetectedRight == 1){
				USStopTimeRight = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
				if(USStartTimeRight < USStopTimeRight){
					UStimeDifferenceRight = USStopTimeRight - USStartTimeRight;
				}
				else{
					UStimeDifferenceRight = (0xffff - USStartTimeRight) + USStopTimeRight;
				}
				USdistanceRight = UStimeDifferenceRight * 0.034 / 2.0;

				USrisingEdgeDetectedRight = 0;
				__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			}
		}

//US Sensor Right END

//Lezer BEGIN
	if(htim->Instance == Letim->Instance && htim->Channel ==HAL_TIM_ACTIVE_CHANNEL_3){
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
