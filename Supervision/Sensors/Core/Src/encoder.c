#include "encoder.h"

#define sysCLK 72000000.0
#define PSC 3599.0
#define oneRot 10.35 //one hole rotation is millimeters
#define W 0.146 //width of car in meters

volatile int speed1en; //Left
volatile int speed2en; //Right
volatile int encoderSumOfRotations1; //Left
volatile int encoderSumOfRotations2; //Right
volatile int timerCntrVal1; //Left
volatile int timerCntrVal2; //Right

volatile int rotChange1; //Left
volatile int rotChange2; //Right


volatile position pos = {0, 0};
volatile double angle = 0; //radian

void Encoder_Init()
{
	encoderSumOfRotations1 = 0;
	encoderSumOfRotations2 = 0;
	speed1en = 0;
	speed2en = 0;
	Timer6_Start();
	Timer7_Start();
}

void Timer6_Start() //Left
{
	HAL_TIM_Base_Start_IT(&htim6);
}

void Timer7_Start() //Right
{
	HAL_TIM_Base_Start_IT(&htim7);
}

void Timer6_Stop() //Left
{
	HAL_TIM_Base_Stop_IT(&htim6);
}

void Timer7_Stop() //Right
{
	HAL_TIM_Base_Stop_IT(&htim7);
}

void WriteEncoderToPC()
{
	//__disable_irq();

	position tmp;
	if(rotChange1 > 5 || rotChange2 > 5)
	{
		tmp = GetPositionFromOrigin();
	}
	printf("Distance right: %f, Speed right: %f, Distance left: %f, Speed left: %f, Position from origin: %f, %f, Angle: %f\n", GetDistanceOfMotor(1), GetSpeedOfMotor(1), GetDistanceOfMotor(0), GetSpeedOfMotor(0), tmp.x, tmp.y, angle);

	//__enable_irq();
}

  double CalculateSpeed(int cntrVal, int speedEn)
  {
	  double timeOfStep = 1.0/(sysCLK / PSC);

	  if(speedEn)
		  return 0.05175/(timeOfStep*cntrVal);
	  else
		  return 0;
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

  position GetPositionFromOrigin() // x: [m], y: [m]
  {
	  double posChgFwd;
	  double angleTMP = angle;
	  if (rotChange1 > rotChange2)
	  {
		  posChgFwd = rotChange2*oneRot/1000;
		  angle -= ((rotChange1-rotChange2)*oneRot/1000)/W;
	  }
	  else
	  {
		  posChgFwd = rotChange1*oneRot/1000;
		  angle += ((rotChange2-rotChange1)*oneRot/1000)/W;
	  }
	  // correction because of rot
	  pos.x += cos((angle+angleTMP)/2)*posChgFwd;
	  pos.y += sin((angle+angleTMP)/2)*posChgFwd;
	  pos.x += W*M_PI*((angle-angleTMP)/2*M_PI)*cos((M_PI-angle-angleTMP)/2);
	  pos.y += W*M_PI*((angle-angleTMP)/2*M_PI)*sin((M_PI-angle-angleTMP)/2);

	  rotChange1 = 0;
	  rotChange2 = 0;

	  return pos;
  }

  void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
  {
	  if (GPIO_Pin == GPIO_PIN_3) //Left
	  {
		  if (getLeftMotorValue() > 0)
		  {
			  encoderSumOfRotations1++;
			  rotChange1++;
		  }
		  else
		  {
			  encoderSumOfRotations1--;
			  rotChange1--;
		  }

		  if(encoderSumOfRotations1 % 5 == 0)
		  {
			  speed1en = 1;
			  Timer6_Stop();
			  timerCntrVal1 = TIM6->CNT;
			  TIM6->CNT = 0;
			  Timer6_Start();
		  }
	  }
	  if (GPIO_Pin == GPIO_PIN_4) //Right
	  {
		  if (getLeftMotorValue() > 0)
		  {
			  encoderSumOfRotations2++;
			  rotChange2++;
		  }
		  else
		  {
			  encoderSumOfRotations2--;
			  rotChange2--;
		  }

		  if(encoderSumOfRotations2 % 5 == 0)
		  {
			  speed2en = 1;
			  Timer7_Stop();
			  timerCntrVal2 = TIM7->CNT;
			  TIM7->CNT = 0;
			  Timer7_Start();
		  }
	  }
  }
