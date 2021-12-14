/*
 * velocity
 * linesensor
 * lasersensor
 * US sensor
 *
 * define
 * 		line detected again
 * 		line leaving
 *
 * very bad:
 * 		if (no obstacle && speed==0)
 * 		if(obstacle && speed>0)
 * 		if(line not detected)
 *
 * other:
 * 		if (go away from line centre)
 * 		if(coming close to the line centre)
 *
 *
 */
#include "reward.h"

extern osSemaphoreId_t SemRewardHandle;

// Variables
uint32_t PrevLightSensor = 0;
uint8_t PrevPos = 0;
int8_t PrevAnalysedMove = 0;
int8_t moveAnalysisResult = 0;
double savedAngle = 0;

extern TIM_HandleTypeDef htim14;

void setMoveAnalysisResult(int8_t value){
	osSemaphoreAcquire(SemRewardHandle, osWaitForever);
	moveAnalysisResult = value;
	osSemaphoreRelease(SemRewardHandle);
	return;
}

int8_t getMoveAnalysisResult(void){
	int8_t retVal = 0;
	osSemaphoreAcquire(SemRewardHandle, osWaitForever);
	retVal = moveAnalysisResult;
	osSemaphoreRelease(SemRewardHandle);
	return retVal;
}

void SetLightSensorValueForTheFirstTime()
{
	PrevLightSensor = GetLightSensorValues();
	moveAnalysis(PrevLightSensor);
}

// return -1 if getting further, return 2 if line is in the middle, return 1 if getting closer, return -2 if no line detected, else 0
int8_t moveAnalysis(uint32_t LightSensor){
	int8_t ret = 0; //nothing changed by default

	uint8_t leftIdx = 0;
	uint8_t rightIdx = 0;

	for(uint8_t i = 0; i < 32; i++){
		if((LightSensor << i) & 0x80000000){
			if(leftIdx == 0){
				leftIdx = i;
			}
			else{
				rightIdx = i;
			}
		}
		else{
			if(rightIdx < leftIdx){
				leftIdx = 0;
				rightIdx = 0;
			}
			if(rightIdx != 0) break;
		}
	}

	//If no line detected
	if(rightIdx == 0){
		 //TODO Sem
		setMoveAnalysisResult(-2);
		return -2;
	}

	uint8_t newPos = leftIdx + rightIdx;

	if (abs(newPos - 31) > abs(PrevPos - 31)) // moving further
		ret = -1;
	else if (abs(newPos - 31) < abs(PrevPos - 31)) // getting closer
		ret = 1;

	if(newPos == 30 || newPos == 31 || newPos == 32)
		ret = 2;

	PrevPos = newPos;

	setMoveAnalysisResult(ret); // TODO SEm
	return ret;
}

int GetReward ()
{
	// Variables
	int reward = 0;

	// Sensor values
	double VelocityLeft = GetSpeedOfMotor(LEFT_SIDE); // m/s
	double VelocityRight = GetSpeedOfMotor(RIGHT_SIDE); // m/s
	uint32_t LaserDistance = getLaserDistance(); //mm
	uint32_t USLeft = getUSDistanceLeft(); //cm
	uint32_t USRight = getUSDistanceRight(); //cm

	// Calculated values
	uint8_t obstacle = (LaserDistance < 500 || USLeft < 50 || USRight < 50);
	 //TODO Sem
	int8_t analysedMove = getMoveAnalysisResult();

	// Calculate reward
	if(obstacle && (VelocityLeft > 0.01 || VelocityRight > 0.01))
		reward += OBSTACLE_AND_RUNNING;

	if(!obstacle && (VelocityLeft < 0.01 && VelocityRight < 0.01))
		reward += NO_OBSTACLE_AND_NO_RUNNING;

	if(analysedMove == -1){
		reward += MOVING_AWAY;
	}
	else if(analysedMove == 1){
		reward += MOVING_CLOSER;
	}
	else if(analysedMove == 2){
		reward += MOVING_MIDDLE;
	}
	else if(analysedMove == -2){
		reward += MOVING_OFF_THE_LINE;
	}

	if(PrevAnalysedMove == -2 && analysedMove != -2){
		reward += LINE_FOUND;
	}
	else if(PrevAnalysedMove != -2 && analysedMove == -2){
		reward += LINE_LOSTED;
	}

	PrevAnalysedMove = analysedMove;

	// printf("Analysed move: %d, Reward: %d\n", analysedMove, reward);

	return reward;


}

uint8_t onTheTrack(void){
	if(getMoveAnalysisResult() == -2){ //TODO Sem
		return 0;
	}
	return 1;
}

void trackLost(void){
	savedAngle = getEuler().x;

	TIM14->CNT = 0;
	HAL_TIM_Base_Start(&htim14);

	leftMotor(-MOTOR_NORMAL);
	rightMotor(-MOTOR_NORMAL);
}

// 0 if going backward, 1 if successfully back, -1 if 3sec is over
int8_t returnToLine(void){

	double deltaAngle;
	uint32_t LEDs = GetLightSensorValues();
	uint8_t lineIsInTheMiddle = 0;

	uint32_t startVal = 0x00040000;
	for (uint8_t i = 0; i < 6; i++)
	{
		if((startVal >> i) & LEDs){
			lineIsInTheMiddle++;
		}

	}

	// successfully back
	if(lineIsInTheMiddle >= 2)
	{
		HAL_TIM_Base_Stop(&htim14);
		leftMotor(MOTOR_STOP);
		rightMotor(MOTOR_STOP);
		return 1;
	}

	// 3 sec is over
	if(TIM14->CNT > 30000)
	{
		HAL_TIM_Base_Stop(&htim14);
		if (LEDs == 0)
		{
			leftMotor(MOTOR_STOP);
			rightMotor(MOTOR_STOP);
			return -1;
		}
		else
		{
			TIM14->CNT = 0;
			HAL_TIM_Base_Start(&htim14);
		}
	}

	  deltaAngle = getEuler().x - savedAngle;
	  if(deltaAngle > 180){
		  deltaAngle -= 360;
	  }
	  else if(deltaAngle < -180){
		  deltaAngle += 360;
	  }

	  // compare the 5 options
	  if (deltaAngle < -7.5)
	  {
		  if (deltaAngle < -15)
		  {
			  leftMotor(-MOTOR_SLOWEST);
			  rightMotor(-MOTOR_FAST);
		  }
		  else
		  {
			  leftMotor(-MOTOR_SLOW);
			  rightMotor(-MOTOR_NORMAL);
		  }
	  }
	  else if (deltaAngle > 7.5)
	  {
		  if (deltaAngle > 15)
		  {
			  leftMotor(-MOTOR_FAST);
			  rightMotor(-MOTOR_SLOWEST);
		  }
		  else
		  {
			  leftMotor(-MOTOR_NORMAL);
			  rightMotor(-MOTOR_SLOW);
		  }
	  }
	  else
	  {
		  leftMotor(-MOTOR_NORMAL);
		  rightMotor(-MOTOR_NORMAL);
	  }

	return 0;
}
