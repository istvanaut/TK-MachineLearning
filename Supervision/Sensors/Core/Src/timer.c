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

enum US_SENSOR{
	LEFT, RIGHT
};

volatile enum US_SENSOR currentUSSensor = LEFT;

//US Sensor END


//Lezer Sensor BEGIN
extern uint32_t Letimed ;
extern uint32_t LeaTime;
extern uint32_t LeoTime;
extern uint32_t Ledistance;
extern uint8_t LeEdgeDetected;


extern TIM_HandleTypeDef* Letim;

//Lezer Sensor END

// Encoder BEGIN
extern int speed1en;
extern int speed2en;
// Encoder END


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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//Left encoder BEGIN
	if(htim->Instance == TIM6)
	{
		speed1en = 0;
	}
	//Left encoder END

	//Right encoder BEGIN
	if(htim->Instance == TIM7)
	{
		speed2en = 0;
	}
	//Right encoder END

	//US Sensor BEGIN
	if(htim->Instance == UStim){
		//LEFT
		if(currentUSSensor == LEFT){
			HAL_TIM_Base_Stop_IT(UStim);
			//Stop LEFT
			HAL_TIM_PWM_Stop(UStim, TIM_CHANNEL_4);
			HAL_TIM_IC_Stop_IT(UStim, TIM_CHANNEL_3);
			//If echo not detected, distance set to 500 cm
			if(USrisingEdgeDetectedLeft == 1) USdistanceLeft = 500;
			USrisingEdgeDetectedLeft = 0;
			//Set timer to zero
			__HAL_TIM_SET_COUNTER(UStim, 0);
			//Set RIGHT
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
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
			if(USrisingEdgeDetectedRight == 1) USdistanceRight = 500;
			USrisingEdgeDetectedRight = 0;
			//Set timer to zero
			__HAL_TIM_SET_COUNTER(UStim, 0);
			//Set LEFT
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
			//Start LEFT
			HAL_TIM_IC_Start_IT(UStim, TIM_CHANNEL_3);
			HAL_TIM_PWM_Start(UStim, TIM_CHANNEL_4);
			HAL_TIM_Base_Start_IT(UStim);

			currentUSSensor = LEFT;
		}
	}
	//US Sensor END
}
