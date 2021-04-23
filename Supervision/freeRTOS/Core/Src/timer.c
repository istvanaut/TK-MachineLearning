#include "main.h"

//US Sensor BEGIN

extern TIM_HandleTypeDef* UStim;

//US Sensor END


//Lezer Sensor BEGIN

extern TIM_HandleTypeDef* Letim;

//Lezer Sensor END


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

//US Sensor
	if(htim->Instance == UStim->Instance){
		USSensorInputCaptureCallback(htim);
	}

//Laser Sensor
	if(htim->Instance == Letim->Instance){
		LaserSensorInputCaptureCallback(htim);
	}

}
