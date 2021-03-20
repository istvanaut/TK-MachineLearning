#include "main.h"
#include "stdint.h"

void Motors_Init(TIM_HandleTypeDef *htim);
void leftMotor(float value);
void rightMotor(float value);
double getLeftMotorValue();
double getRightMotorValue();
