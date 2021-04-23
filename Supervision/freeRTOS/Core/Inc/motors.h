#include "main.h"
#include "stdint.h"

void Motors_Init(TIM_HandleTypeDef *htim);
void leftMotor(double value);
void rightMotor(double value);
double getLeftMotorValue();
double getRightMotorValue();
