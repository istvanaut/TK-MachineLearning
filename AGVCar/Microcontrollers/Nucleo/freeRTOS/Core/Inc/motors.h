#ifndef MOTORS_H
#define MOTORS_H

#include "main.h"
#include "stdint.h"

typedef enum motorState{
	MOTOR_DISABLE, MOTOR_ENABLE
}motorState;

void Motors_Init(TIM_HandleTypeDef *htim);
void leftMotor(double value);
void rightMotor(double value);
double getLeftMotorValue();
double getRightMotorValue();
void setMotorEnable(motorState value);


#endif
