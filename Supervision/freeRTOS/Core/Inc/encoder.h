#ifndef ENCODER_H_
#define ENCODER_H_

#include "main.h"
#include "myCOM.h"
#include "math.h"

extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

typedef struct Pos
{
	double x;
	double y;
}position;

void Encoder_Init();

void Timer6_Start();
void Timer7_Start();
void Timer6_Stop();
void Timer7_Stop();

void GetEncoderData();

int GetLeftDistance();
int GetRightDistance();
double GetActualSpeed();
double GetActualPositionFromOrigin();

double CalculateSpeed(int cntrVal, int speedEn);

double GetSpeedOfMotor(int motor); // 0 => left, 1 => right [m/s]
double GetDistanceOfMotor(int motor); // 0 => left, 1 => right [m]
position GetPositionFromOrigin(); // x: [m], y: [m]

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif
