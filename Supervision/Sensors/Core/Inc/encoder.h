#ifndef ENCODER_H_
#define ENCODER_H_

#include "main.h"
#include "myCOM.h"
#include "math.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

typedef struct Pos
{
	double x;
	double y;
}position;

void Encoder_Init();

void Timer3_Start();
void Timer4_Start();
void Timer3_Stop();
void Timer4_Stop();

void WriteEncoderToPC();
double CalculateSpeed(int cntrVal, int speedEn);

double GetSpeedOfMotor(int motor); // 0 => left, 1 => right [m/s]
double GetDistanceOfMotor(int motor); // 0 => left, 1 => right [m]
position GetPositionFromOrigin(); // x: [m], y: [m]

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif
