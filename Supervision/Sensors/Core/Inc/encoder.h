#include "stm32f7xx_hal.h"
#include "myCOM.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

void Encoder_Init();

void Timer3_Start();
void Timer4_Start();
void Timer3_Stop();
void Timer4_Stop();
void Timer3_CntrVal();
void Timer4_CntrVal();

void WriteEncoderToPC();
double CalculateSpeed(int cntrVal, int speedEn);

double GetSpeedOfMotor(int motor); // 0 => left, 1 => right [m/s]
double GetDistanceOfMotor(int motor); // 0 => left, 1 => right [m]
double GetPositionFromOrigin(); // x: [m], y: [m]

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
