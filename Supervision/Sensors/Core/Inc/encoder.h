#include "stm32f7xx_hal.h"
#include "myCOM.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

extern int speed1en;
extern int speed2en;
extern int encoderSumOfRotations1;
extern int encoderSumOfRotations2;
extern int timerCntrVal1;
extern int timerCntrVal2;

void Encoder_Init();

void Timer3_Start();
void Timer4_Start();
void Timer3_Stop();
void Timer4_Stop();
void Timer3_CntrVal();
void Timer4_CntrVal();

void WriteEncoderToPC(int dr, int dl, int sr, int sl);
double CalculateSpeed(int cntrVal, int speedEn);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
