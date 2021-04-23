#ifndef ENCODER_H_
#define ENCODER_H_

#include "main.h"
#include "myCOM.h"
#include "math.h"

#define LEFT_SIDE 0
#define RIGHT_SIDE 1

extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

typedef struct Pos
{
	double x;
	double y;
}position;

// Basic handle functions
void Encoder_Init();

void Timer6_Start();
void Timer7_Start();
void Timer6_Stop();
void Timer7_Stop();

// Set functions
void DisableSpeed(int motor); // 0 => left, 1 => right

// Calculate functions
void CalculateDistance(int motor);
void CalculateSpeed(int motor);
void CalculatePositionAndAngle();

// Debug functions
void PrintEncoderAllData(); //DEBUGHOZ

// Get functions
double GetSpeedOfMotor(int motor); // 0 => left, 1 => right [m/s]
double GetDistanceOfMotor(int motor); // 0 => left, 1 => right [m]
position GetPositionFromOrigin(); // x: [m], y: [m]

// EXTI functions
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif
