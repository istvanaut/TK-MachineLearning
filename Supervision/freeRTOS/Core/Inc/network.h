/*
 * network.h
 *
 *  Created on: Apr 23, 2021
 *      Author: kolon
 */

#ifndef INC_NETWORK_H_
#define INC_NETWORK_H_

#include "main.h"
#include "simplenn.h"
#include "simplenn_data.h"
#include "math.h"

#define LEFT_MOTOR_SPEED_SCALER		0.8f
#define RIGHT_MOTOR_SPEED_SCALER	0.8f
/*Define sample time*/
#define CONTROL_DELAY				2000
#define TIME_LIMIT					30  //in seconds
#define MAX_CYCLES_0				10 //300

#define EPS_START 					0.9f
#define EPS_END  					0.05f               // with 2 choices this means 0.4/2 -> 20% are wrong random choices (should be tolerable)
#define EPS_DECAY 					200                  // This should equal a couple of short runs
/*define main states*/
#define INIT        				0u
#define LOAD        				1u
#define RUNNING     				2u

/*Defines for inter MCU communication*/
#define CAM_WIDTH   				64u
#define CAM_HEIGHT 					64u
#define ESP_RESP_OK					1u
#define ESP_RESP_NOK				0u

#define ESP_REQUEST_WEIGHTS			0x01u
#define ESP_REQUEST_CAMERA_FRAME	0x02u
#define ESP_SEND_STATE_1			0x03u
#define ESP_SEND_STATE_2			0x04u
#define ESP_PING					0x05u

#define CAMERA_WIDTH				32
#define CAMERA_HEIGHT				32

/*define if you want to log AI related data*/
//#define AILOG
//#define TEST
//#define LOGGING
#define HAS_CAMERA

typedef struct
{
	/*Sensor states*/
	uint32_t lightsensor;
	uint32_t usLeft;
	uint32_t usRight;
	uint32_t laser;
	/*Performed action*/
	float leftM;
	float rightM;
	float reward;
}sensorSatate;

int aiInit(void);
int aiRun(const void *in_data, void *out_data);
HAL_StatusTypeDef transmitToESP(uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef receiveFromESP(uint8_t *pData, uint16_t Size, uint32_t Timeout);
void initStateHandle(void);
void loadStateHandle(void);
float calculateReward(sensorSatate* data);
void runningStateHandle(void);
void networkSwitch(void);


#endif /* INC_NETWORK_H_ */
