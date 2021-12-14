/*
 * motors.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Kristof
 */

#include "motors.h"

#define ARR 6751u

extern osSemaphoreId_t SemMotorsHandle;

TIM_HandleTypeDef* myhtim;
static double leftMotorActualValue = 0;
static double rightMotorActualValue = 0;
static motorState motorEnable = MOTOR_DISABLE;

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

void leftMotor(double value)
{
	motorState tmp;
	osSemaphoreAcquire(SemMotorsHandle, osWaitForever);
	leftMotorActualValue = value;
	tmp = motorEnable;
	osSemaphoreRelease(SemMotorsHandle);

	if(tmp == MOTOR_DISABLE)
		value = 0;

    // value > 0 --> forward, value < 0 --> backward
	myhtim->Instance->CCR1 = (uint32_t) (value > 0 ? abs(ARR * value) : 0);
	myhtim->Instance->CCR2 = (uint32_t) (value > 0 ? 0 : abs(ARR * value));
}
void rightMotor(double value)
{
	motorState tmp;
	osSemaphoreAcquire(SemMotorsHandle, osWaitForever);
	rightMotorActualValue = value;
	tmp = motorEnable;
	osSemaphoreRelease(SemMotorsHandle);

	if(tmp == MOTOR_DISABLE)
			value = 0;

    // value > 0 --> forward, value < 0 --> backward
	myhtim->Instance->CCR3 = (uint32_t) (value > 0 ? abs(ARR * value) : 0);
	myhtim->Instance->CCR4 = (uint32_t) (value > 0 ? 0 : abs(ARR * value));
}

double getLeftMotorValue()
{
	double retTemp;

	osSemaphoreAcquire(SemMotorsHandle, osWaitForever);
	retTemp = leftMotorActualValue;
	osSemaphoreRelease(SemMotorsHandle);

	return retTemp;
}

double getRightMotorValue()
{
	double retTemp;

	osSemaphoreAcquire(SemMotorsHandle, osWaitForever);
	retTemp = rightMotorActualValue;
	osSemaphoreRelease(SemMotorsHandle);

	return retTemp;
}

void setMotorEnable(motorState value){
	osSemaphoreAcquire(SemMotorsHandle, osWaitForever);
	motorEnable = value;
	osSemaphoreRelease(SemMotorsHandle);

	return;
}
