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



int GetReward ()
{
	int reward = 0;
	uint32_t LightSensor = GetLightSensorValues(); // MSB:left side LSB right side
	double VelocityLeft = GetSpeedOfMotor(LEFT_SIDE); // m/s
	double VelocityRight = GetSpeedOfMotor(RIGHT_SIDE); // m/s

	if()

	return reward;


}
