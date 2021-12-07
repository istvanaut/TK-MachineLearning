#include "encoder.h"

#define sysCLK 72000000.0
#define PSC 600.0
#define speedCalcPeriod 3
#define oneRot 10.35 //one hole rotation is millimeters
#define W 0.146 //width of car in meters

// Semaphores
extern osSemaphoreId_t SemLeftEncoderHandle;
extern osSemaphoreId_t SemRightEncoderHandle;
extern osSemaphoreId_t SemPosHandle;

// Variables
static volatile int speedLeftEN; //Left
static volatile int speedRightEN; //Right
static volatile int encoderSumOfRotationsLeft; //Left
static volatile int encoderSumOfRotationsRight; //Right
static volatile int timerCntrValLeft; //Left
static volatile int timerCntrValRight; //Right

static volatile int rotChangeLeft; //Left
static volatile int rotChangeRight; //Right

// Motor values
static double speedLeft = 0;
static double speedRight = 0;
static double distanceLeft = 0;
static double distanceRight = 0;
static position pos = {0, 0};
static double angle = 0; //radian


// Basic handle functions
void Encoder_Init()
{
	encoderSumOfRotationsLeft = 0;
	encoderSumOfRotationsRight = 0;
	speedLeftEN = 0;
	speedRightEN = 0;
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

// Set functions
void DisableSpeed(int motor) // 0 => left, 1 => right
{
	if (motor == LEFT_SIDE)
	{
		if(osSemaphoreAcquire(SemLeftEncoderHandle, 0) == osOK)
		{
			speedLeftEN = 0;
			osSemaphoreRelease(SemLeftEncoderHandle);
		}
	}
	else
	{
		if(osSemaphoreAcquire(SemRightEncoderHandle, 0) == osOK)
		{
			speedRightEN = 0;
			osSemaphoreRelease(SemRightEncoderHandle);
		}
	}
}

// Calculate functions
void CalculateDistance(int motor)
{
	if (motor == LEFT_SIDE)
	{
		osSemaphoreAcquire(SemLeftEncoderHandle, osWaitForever);
		distanceLeft = encoderSumOfRotationsLeft*oneRot/1000;
		osSemaphoreRelease(SemLeftEncoderHandle);
	}
	else
	{
		osSemaphoreAcquire(SemRightEncoderHandle, osWaitForever);
		distanceRight = encoderSumOfRotationsRight*oneRot/1000;
		osSemaphoreRelease(SemRightEncoderHandle);
	}
}

void CalculateSpeed(int motor)
{
	  double timeOfStep = 1.0/(sysCLK / PSC);

	  if (motor == LEFT_SIDE)
	  {
		  osSemaphoreAcquire(SemLeftEncoderHandle, osWaitForever);
		  if(speedLeftEN)
			  speedLeft = (speedCalcPeriod * oneRot / 1000)/(timeOfStep*timerCntrValLeft); //TMRCNTRVAL csak ha mind egy irányba?
		  else
			  speedLeft = 0;
		  osSemaphoreRelease(SemLeftEncoderHandle);
	  }
	  else
	  {
		  osSemaphoreAcquire(SemRightEncoderHandle, osWaitForever);
		  if(speedRightEN)
			  speedRight = (speedCalcPeriod * oneRot / 1000)/(timeOfStep*timerCntrValRight); //TMRCNTRVAL csak ha mind egy irányba?
		  else
			  speedRight = 0;
		  osSemaphoreRelease(SemRightEncoderHandle);
	  }

}

void CalculatePositionAndAngle()
{
	osSemaphoreAcquire(SemPosHandle, osWaitForever);
	osSemaphoreAcquire(SemLeftEncoderHandle, osWaitForever);
	osSemaphoreAcquire(SemRightEncoderHandle, osWaitForever);

	  double posChgFwd;
	  double angleTMP = angle;
	  if (rotChangeLeft > rotChangeRight)
	  {
		  posChgFwd = rotChangeRight*oneRot/1000;
		  angle -= ((rotChangeLeft-rotChangeRight)*oneRot/1000)/W;
	  }
	  else
	  {
		  posChgFwd = rotChangeLeft*oneRot/1000;
		  angle += ((rotChangeRight-rotChangeLeft)*oneRot/1000)/W;
	  }
	  // correction because of rot
	  pos.x += cos((angle+angleTMP)/2)*posChgFwd;
	  pos.y += sin((angle+angleTMP)/2)*posChgFwd;
	  pos.x += W*M_PI*((angle-angleTMP)/2*M_PI)*cos((M_PI-angle-angleTMP)/2);
	  pos.y += W*M_PI*((angle-angleTMP)/2*M_PI)*sin((M_PI-angle-angleTMP)/2);

	  rotChangeLeft = 0;
	  rotChangeRight = 0;

	osSemaphoreRelease(SemRightEncoderHandle);
	osSemaphoreRelease(SemLeftEncoderHandle);
	osSemaphoreRelease(SemPosHandle);
}

// Debug functions
void PrintEncoderAllData()
{
	osSemaphoreAcquire(SemPosHandle, osWaitForever);
	osSemaphoreAcquire(SemLeftEncoderHandle, osWaitForever);
	osSemaphoreAcquire(SemRightEncoderHandle, osWaitForever);
	printf("Distance SUM right: %f, Speed right: %f, Distance SUM left: %f, Speed left: %f\n\rPosition from origin: %f, %f, Angle in degrees: %f\n", distanceRight, speedRight, distanceLeft, speedLeft, pos.x, pos.y, angle*360/(2*M_PI));
	osSemaphoreRelease(SemRightEncoderHandle);
	osSemaphoreRelease(SemLeftEncoderHandle);
	osSemaphoreRelease(SemPosHandle);
}

// Get functions
  double GetSpeedOfMotor(int motor) // 0 => left, 1 => right [m/s]
  {
	  double retTemp;
	  if(motor == LEFT_SIDE)
	  {
		  osSemaphoreAcquire(SemLeftEncoderHandle, osWaitForever);
		  retTemp = speedLeft;
		  osSemaphoreRelease(SemLeftEncoderHandle);
	  }
	  else
	  {
		  osSemaphoreAcquire(SemRightEncoderHandle, osWaitForever);
		  retTemp = speedRight;
		  osSemaphoreRelease(SemRightEncoderHandle);
	  }
	  return retTemp;
  }

  double GetDistanceOfMotor(int motor) // 0 => left, 1 => right [m]
  {
	  double retTemp;
	  if(motor == LEFT_SIDE)
	  {
		  osSemaphoreAcquire(SemLeftEncoderHandle, osWaitForever);
		  retTemp = distanceLeft;
		  osSemaphoreRelease(SemLeftEncoderHandle);
	  }
	  else
	  {
		  osSemaphoreAcquire(SemRightEncoderHandle, osWaitForever);
		  retTemp = distanceRight;
		  osSemaphoreRelease(SemRightEncoderHandle);
	  }
	  return retTemp;
  }

  position GetPositionFromOrigin() // x: [m], y: [m]
  {
	  position retTemp;

	  osSemaphoreAcquire(SemPosHandle, osWaitForever);
	  retTemp = pos;
	  osSemaphoreRelease(SemPosHandle);

	  return retTemp;
  }

  // EXTI functions
  void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
  {
	  if (GPIO_Pin == GPIO_PIN_3) //Left
	  {
		  if(osSemaphoreAcquire(SemLeftEncoderHandle, 0) == osOK) // Take semaphore
		  {
			  if (getLeftMotorValue() > 0)
			  {
				  encoderSumOfRotationsLeft++;
				  rotChangeLeft++;
			  }
			  else
			  {
				  encoderSumOfRotationsLeft--;
				  rotChangeLeft--;
			  }

			  if(encoderSumOfRotationsLeft % speedCalcPeriod == 0)
			  {
				  speedLeftEN = 1;
				  Timer6_Stop();
				  timerCntrValLeft = TIM6->CNT;
				  TIM6->CNT = 0;
				  Timer6_Start();
			  }
			  osSemaphoreRelease(SemLeftEncoderHandle); // Release semaphore
		  }
	  }
	  if (GPIO_Pin == GPIO_PIN_4) //Right
	  {
		  if(osSemaphoreAcquire(SemRightEncoderHandle, 0) == osOK) // Take semaphore
		  {
			  if (getLeftMotorValue() > 0)
			  {
				  encoderSumOfRotationsRight++;
				  rotChangeRight++;
			  }
			  else
			  {
				  encoderSumOfRotationsRight--;
				  rotChangeRight--;
			  }

			  if(encoderSumOfRotationsRight % speedCalcPeriod == 0)
			  {
				  speedRightEN = 1;
				  Timer7_Stop();
				  timerCntrValRight = TIM7->CNT;
				  TIM7->CNT = 0;
				  Timer7_Start();
			  }
			  osSemaphoreRelease(SemRightEncoderHandle); // Release semaphore
		  }
	  }
  }
