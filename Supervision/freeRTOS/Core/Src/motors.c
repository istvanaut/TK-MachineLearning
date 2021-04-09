/*
 * motors.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Kristof
 */

#include "motors.h"

#define ARR 2251u

extern osSemaphoreId_t SemMotorsHandle;

TIM_HandleTypeDef* myhtim;
static double leftMotorActualValue = 0;
static double rightMotorActualValue = 0;

void Motors_Init(TIM_HandleTypeDef *htim)
{
	 myhtim = htim;
	 //start pwm
	 //left motor
	 HAL_TIM_PWM_Start(myhtim, TIM_CHANNEL_1);
	 HAL_TIM_PWM_Start(myhtim, TIM_CHANNEL_2);
	 //rigth motor
	 HAL_TIM_PWM_Start(myhtim, TIM_CHANNEL_3);
	 HAL_TIM_PWM_Start(myhtim, TIM_CHANNEL_4);

     //init to zero
	 myhtim->Instance->CCR1 = 0u;
	 myhtim->Instance->CCR2 = 0u;
	 myhtim->Instance->CCR3 = 0u;
	 myhtim->Instance->CCR4 = 0u;
}

void leftMotor(float value)
{
	osSemaphoreAcquire(SemMotorsHandle, 0);
	leftMotorActualValue = value;
	osSemaphoreRelease(SemMotorsHandle);

    // value > 0 --> forward, value < 0 --> backward
	myhtim->Instance->CCR1 = (uint32_t) (value > 0 ? ARR * value : 0);
	myhtim->Instance->CCR2 = (uint32_t) (value > 0 ? 0 : ARR * value);
}
void rightMotor(float value)
{
	osSemaphoreAcquire(SemMotorsHandle, 0);
	rightMotorActualValue = value;
	osSemaphoreRelease(SemMotorsHandle);

    // value > 0 --> forward, value < 0 --> backward
	myhtim->Instance->CCR3 = (uint32_t) (value > 0 ? ARR * value : 0);
	myhtim->Instance->CCR4 = (uint32_t) (value > 0 ? 0 : ARR * value);
}

double getLeftMotorValue()
{
	double retTemp;

	osSemaphoreAcquire(SemMotorsHandle, 0);
	retTemp = leftMotorActualValue;
	osSemaphoreRelease(SemMotorsHandle);

	return retTemp;
}

double getRightMotorValue()
{
	double retTemp;

		osSemaphoreAcquire(SemMotorsHandle, 0);
		retTemp = rightMotorActualValue;
		osSemaphoreRelease(SemMotorsHandle);

		return retTemp;
}
