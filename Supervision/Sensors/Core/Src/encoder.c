#include "encoder.h"

#define sysCLK 72000000
#define PSC 3599
#define oneRot 10.35

int speed1en; //Left
int speed2en; //Right
int encoderSumOfRotations1; //Left
int encoderSumOfRotations2; //Right
int timerCntrVal1; //Left
int timerCntrVal2; //Right

/*double distanceLeft; //Left
double distanceRight; //Right
double speedLeft; //Left
double speedRight; //Right*/

void Encoder_Init()
{
	encoderSumOfRotations1 = 0;
	encoderSumOfRotations2 = 0;
	speed1en = 0;
	speed2en = 0;
	Timer3_Start();
	Timer4_Start();
}

void Timer3_Start() //Left
  {
  	HAL_TIM_Base_Start_IT(&htim3);
  }

  void Timer4_Start() //Right
  {
  	HAL_TIM_Base_Start_IT(&htim4);
  }

  void Timer3_Stop() //Left
  {
  	HAL_TIM_Base_Stop_IT(&htim3);
  }

  void Timer4_Stop() //Right
  {
  	HAL_TIM_Base_Stop_IT(&htim4);
  }

  void Timer3_CntrVal() //Left
  {
	  timerCntrVal1 = TIM3->CNT;
	  TIM3->CNT = 0;
  }

  void Timer4_CntrVal() //Right
  {
	  timerCntrVal2 = TIM4->CNT;
	  TIM4->CNT = 0;
  }

  void WriteEncoderToPC()
  {
	  printf("Distance right: %f, Speed right: %f, Distance left: %f, Speed left: %f\n", GetDistanceOfMotor(1), GetSpeedOfMotor(1), GetDistanceOfMotor(0), GetSpeedOfMotor(0));
  }

  double CalculateSpeed(int cntrVal, int speedEn)
  {
	  double timeOfStep = 1/(sysCLK / PSC);
	  if(speedEn)
	  {
		  return timeOfStep*cntrVal;
	  }
	  else
	  {
		  return 0;
	  }
  }

  double GetSpeedOfMotor(int motor) // 0 => left, 1 => right [m/s]
  {
	  if(motor)
		  return CalculateSpeed(timerCntrVal2, speed2en);
	  else
		  return CalculateSpeed(timerCntrVal1, speed1en);
  }

  double GetDistanceOfMotor(int motor) // 0 => left, 1 => right [m]
  {
	  if(motor)
		  return encoderSumOfRotations2*oneRot/1000;
	  else
		  return encoderSumOfRotations1*oneRot/1000;
  }

  double GetPositionFromOrigin() // x: [m], y: [m]
  {
	  return 0;
  }

  void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
  {
    if (GPIO_Pin == GPIO_PIN_3)
    {
  	  encoderSumOfRotations1++;
  	  if(encoderSumOfRotations1 % 5 == 0)
  	  {
  		  speed1en = 1;
  		  Timer3_Stop();
  		  Timer3_CntrVal();
  		  Timer3_Start();
  	  }
    }
    if (GPIO_Pin == GPIO_PIN_4)
    {
  	  encoderSumOfRotations2++;
  	  if(encoderSumOfRotations2 % 5 == 0)
  	  {
  		 speed2en = 1;
  	     Timer4_Stop();
  	     Timer4_CntrVal();
  	     Timer4_Start();
  	  }
    }

  }

  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
    	if(htim->Instance == TIM3)
    	{
    		speed1en = 0;
    	}

    	if(htim->Instance == TIM4)
    	{
    		speed2en = 0;
    	}
    }
