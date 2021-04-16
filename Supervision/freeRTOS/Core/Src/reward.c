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

// Variables
uint32_t PrevLightSensor = 0;
uint8_t PrevPos = 0;
int8_t PrevAnalysedMove = 0;

int8_t moveAnalysis(uint32_t LightSensor);

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

	return ret;
}

int GetReward ()
{
	// Variables
	int reward = 0;

	// Sensor values
	uint32_t LightSensor = GetLightSensorValues(); // MSB:left side LSB right side
	double VelocityLeft = GetSpeedOfMotor(LEFT_SIDE); // m/s
	double VelocityRight = GetSpeedOfMotor(RIGHT_SIDE); // m/s
	uint32_t LaserDistance = getLaserDistance(); //mm
	uint32_t USLeft = getUSDistanceLeft(); //cm
	uint32_t USRight = getUSDistanceRight(); //cm

	// Calculated values
	uint8_t obstacle = (LaserDistance < 500 || USLeft < 50 || USRight < 50);
	uint8_t lineLosted = PrevLightSensor && !LightSensor;
	int8_t analysedMove = moveAnalysis(LightSensor);

	// Calculate reward
	if(obstacle && (VelocityLeft > 0.01 || VelocityRight > 0.01))
		reward += OBSTACLE_AND_RUNNING;

	if(!obstacle && (VelocityLeft < 0.01 && VelocityRight < 0.01))
		reward += NO_OBSTACLE_AND_NO_RUNNING;

	if(lineLosted)
		reward += LINE_LOSTED;

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

	PrevAnalysedMove = analysedMove;
	PrevLightSensor = LightSensor;

	printf("Analysed move: %d, Reward: %d\n", analysedMove, reward);

	return reward;


}
