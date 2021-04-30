/*
 * reward.h
 *
 *  Created on: Apr 16, 2021
 *      Author: feket
 */
#include "main.h"

#define OBSTACLE_AND_RUNNING -1000;
#define NO_OBSTACLE_AND_NO_RUNNING -1000;
#define LINE_LOSTED -1000;
#define MOVING_OFF_THE_LINE -10;
#define MOVING_AWAY -10;
#define MOVING_CLOSER 10;
#define MOVING_MIDDLE 20;
#define LINE_FOUND 100;

void SetLightSensorValueForTheFirstTime(void);
int GetReward (void);
uint8_t onTheTrack(void);
void trackLost(void);
int8_t returnToLine(void);
int8_t moveAnalysis(uint32_t LightSensor);
void setMoveAnalysisResult(int8_t value);
int8_t getMoveAnalysisResult(void);
