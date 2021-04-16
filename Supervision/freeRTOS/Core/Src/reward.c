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
 * 		if (go away line)
 * 		if(coming close to the line)
 *
 *
 */
#include "reward.h"

// Variables
uint32_t PrevLightSensor = 0;

void SetLightSensorValueForTheFirstTime()
{
	PrevLightSensor = GetLightSensorValues();
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

	// Calculate reward
	if(obstacle && (VelocityLeft > 0 || VelocityRight > 0))
		reward -= 1000;




	PrevLightSensor = LightSensor;

	return reward;


}
