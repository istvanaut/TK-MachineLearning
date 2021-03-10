#include "encoder.h"

#define sysCLK 72000000
#define PSC 3599

void Encoder_Init()
{
	encoderSumOfRotations1 = 0;
	encoderSumOfRotations2 = 0;
	Timer3_Start();
	Timer4_Start();
}

void Timer3_Start()
  {
  	HAL_TIM_Base_Start_IT(&htim3);
  }

  void Timer4_Start()
  {
  	HAL_TIM_Base_Start_IT(&htim4);
  }

  void Timer3_Stop()
  {
  	HAL_TIM_Base_Stop_IT(&htim3);
  }

  void Timer4_Stop()
  {
  	HAL_TIM_Base_Stop_IT(&htim4);
  }

  void Timer3_CntrVal()
  {
	  timerCntrVal1 = TIM3->CNT;
	  TIM3->CNT = 0;
  }

  void Timer4_CntrVal()
  {
	  timerCntrVal2 = TIM4->CNT;
	  TIM4->CNT = 0;
  }

  void WriteEncoderToPC(int dr, int dl, double sr, double sl)
  {
	  printf("Distance right: %d, Speed right: %f, Distance left: %d, Speed left: %f\n", dr, sr, dl, sl);
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
