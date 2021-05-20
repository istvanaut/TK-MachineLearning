/*
 * network.c
 *
 *  Created on: Apr 23, 2021
 *      Author: kolon
 */

#include "network.h"

#define WEIGHTS_CHUNKS 1024

extern CRC_HandleTypeDef hcrc;
extern RNG_HandleTypeDef hrng;
extern SPI_HandleTypeDef hspi3;
extern UART_HandleTypeDef huart3;

static ai_handle network = AI_HANDLE_NULL;

AI_ALIGNED(32)
static ai_u8 activations[AI_SIMPLENN_DATA_ACTIVATIONS_SIZE];

AI_ALIGNED(32)
static ai_float in_data[AI_SIMPLENN_IN_1_SIZE];

/* Data payload for the output tensor */
AI_ALIGNED(32)
static ai_float out_data[AI_SIMPLENN_OUT_1_SIZE];

uint8_t state;
uint8_t nextState;
uint32_t stepsDone;
uint64_t cycleCounter;

uint8_t camera_frame[CAMERA_WIDTH*CAMERA_HEIGHT];

void networkSwitch(void){
	switch(state){
		case LOAD:
			loadStateHandle();
			break;

		case RUNNING:
			runningStateHandle();
			break;
		default:
			state = RUNNING;
	}
	state = nextState;
	return;
}

int aiInit(void) {
  ai_error err;

#ifdef AILOG
  char buf[100];
  memset(buf,'\0',100);
#endif

  /* 1 - Create an instance of the model */
  err = ai_simplenn_create(&network, AI_SIMPLENN_DATA_CONFIG /* or NULL */);

  if (err.type != AI_ERROR_NONE) {
#ifdef AILOG
	  memset(buf,'\0',100);
	  sprintf(buf, "E: AI ai_network_create error - type=%d code=%d\r\n", err.type, err.code);
	  HAL_Delay(10);
	  HAL_UART_Transmit(&huart3, buf, 100, HAL_MAX_DELAY );
#endif
	  return -1;
    };

  /* 2 - Initialize the instance */
  const ai_network_params params = {
      AI_SIMPLENN_DATA_WEIGHTS(ai_simplenn_data_weights_get()),//ai_simplenn_data_weights_get()),
      AI_SIMPLENN_DATA_ACTIVATIONS(activations) };

  if (!ai_simplenn_init(network, &params)) {
      err = ai_simplenn_get_error(network);
#ifdef AILOG
      memset(buf,'\0',100);
      sprintf(buf, "E: AI ai_network_init error - type=%d code=%d\r\n", err.type, err.code);
      HAL_Delay(10);
      HAL_UART_Transmit(&huart3, buf, 100, HAL_MAX_DELAY );
#endif
      return -1;
    }

  return 0;
}

int aiRun(const void *in_data, void *out_data)
{
  ai_i32 n_batch;
  ai_error err;
#ifdef AILOG
  char buf[100];
  memset(buf,'\0',100);
  sprintf(buf, "Run AI");
  HAL_Delay(10);
  HAL_UART_Transmit(&huart3, buf, 100, HAL_MAX_DELAY );
#endif
  /* 1 - Create the AI buffer IO handlers with the default definition */
  ai_buffer ai_input[AI_SIMPLENN_IN_NUM] = AI_SIMPLENN_IN ;
  ai_buffer ai_output[AI_SIMPLENN_OUT_NUM] = AI_SIMPLENN_OUT ;

  /* 2 - Update IO handlers with the data payload */
  ai_input[0].n_batches = 1;
  ai_input[0].data = AI_HANDLE_PTR(in_data);
  ai_output[0].n_batches = 1;
  ai_output[0].data = AI_HANDLE_PTR(out_data);

  /* 3 - Perform the inference */
  n_batch = ai_simplenn_run(network, &ai_input[0], &ai_output[0]);
  if (n_batch != 1) {
      err = ai_simplenn_get_error(network);
#ifdef AILOG
      memset(buf,'\0',100);
      sprintf(buf, "E: AI ai_network_run error - type=%d code=%d\r\n", err.type, err.code);
      HAL_Delay(10);
      HAL_UART_Transmit(&huart3, buf, 100, HAL_MAX_DELAY );
#endif
      return -1;
  }
  return 0;
}
HAL_StatusTypeDef transmitToESP(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	HAL_StatusTypeDef status;
	// wait for ESP to signal ready
	while(!HAL_GPIO_ReadPin(ESP_RDY_GPIO_Port, ESP_RDY_Pin));
	// pull slave select low
	HAL_GPIO_WritePin(ESP_CS_GPIO_Port, ESP_CS_Pin, GPIO_PIN_RESET);
	// transmit data
	status = HAL_SPI_Transmit(&hspi3, pData, Size, Timeout);
	// pull slave select pin high
	HAL_GPIO_WritePin(ESP_CS_GPIO_Port, ESP_CS_Pin, GPIO_PIN_SET);
	return status;
}
HAL_StatusTypeDef receiveFromESP(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	HAL_StatusTypeDef status;
	// wait for ESP to signal ready
	while(!HAL_GPIO_ReadPin(ESP_RDY_GPIO_Port, ESP_RDY_Pin));
	// pull slave select low
	HAL_GPIO_WritePin(ESP_CS_GPIO_Port, ESP_CS_Pin, GPIO_PIN_RESET);
	// receive data
	status = HAL_SPI_Receive(&hspi3, pData, Size, Timeout);
	// pull slave select pin high
	HAL_GPIO_WritePin(ESP_CS_GPIO_Port, ESP_CS_Pin, GPIO_PIN_SET);
	return status;
}
void initStateHandle(void)
{
	nextState = LOAD;
	stepsDone = 0u;
	aiInit();
}
void loadStateHandle(void){
	char buf[8];
	uint8_t resp;
	uint8_t request;
	HAL_StatusTypeDef status;
	//destroy nn
	network = ai_simplenn_destroy(network);
	if(network != NULL)
	{
		//error
		printf("Network destroy error\n");
		HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD2_Pin|LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
		nextState = INIT;
	}
	else
	{
		/*Request weights from ESP*/
		HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD2_Pin|LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);

		printf("Checking ESP connection...\n");
		request = ESP_PING;

		/*Test connection*/
		do
		{
			transmitToESP(&request, 1, HAL_MAX_DELAY);
			printf("Waiting for esp ACK\n");

			status = receiveFromESP(&resp, sizeof(resp), 200);
			printf("%d\n",resp);
		}while(status!= HAL_OK || resp != ESP_RESP_OK);
		printf("ESP OK\n");


		/* Getting weights*/
		printf("Requesting weights\n");
		//request = ESP_REQUEST_WEIGHTS;
		//transmitToESP(&request, 1, HAL_MAX_DELAY);
		//printf("Request sent\n");

		printf("Starting receiving weights in chunks!\n");
		int k = 0;
		request = ESP__START_REQUEST_WEIGHTS;
		transmitToESP(&request, 1, HAL_MAX_DELAY);
		HAL_Delay(1);
		request = ESP_REQUEST_WEIGHTS;
		for(k = 0; k < AI_SIMPLENN_DATA_WEIGHTS_SIZE / WEIGHTS_CHUNKS; k++)
		{
			transmitToESP(&request, 1, HAL_MAX_DELAY);
			HAL_Delay(1);
			receiveFromESP(ai_simplenn_data_weights_get() + k * WEIGHTS_CHUNKS , WEIGHTS_CHUNKS, HAL_MAX_DELAY);
			/*printf("Chunk %d received\n",k);
			printf("Accessing element: %d\n",k*WEIGHTS_CHUNKS);
			printf("Chunk: %d\n",*(ai_simplenn_data_weights_get() + k * WEIGHTS_CHUNKS));*/
			HAL_Delay(1);
		}
		/*Receive remaining chunk*/
		printf("Remaining chunks: %d\n",AI_SIMPLENN_DATA_WEIGHTS_SIZE - k * WEIGHTS_CHUNKS);
		request = ESP_REQ_LAST_CHUNK;
		HAL_Delay(1);
		if((AI_SIMPLENN_DATA_WEIGHTS_SIZE / WEIGHTS_CHUNKS) * WEIGHTS_CHUNKS < AI_SIMPLENN_DATA_WEIGHTS_SIZE)
		{

			transmitToESP(&request, 1, HAL_MAX_DELAY);
			HAL_Delay(1);
			receiveFromESP(ai_simplenn_data_weights_get() + (AI_SIMPLENN_DATA_WEIGHTS_SIZE / WEIGHTS_CHUNKS) * WEIGHTS_CHUNKS , AI_SIMPLENN_DATA_WEIGHTS_SIZE - (AI_SIMPLENN_DATA_WEIGHTS_SIZE / WEIGHTS_CHUNKS) * WEIGHTS_CHUNKS, HAL_MAX_DELAY);
			printf("Last chunk received\n");
		}

		/*for(int i = 0; i < AI_SIMPLENN_DATA_WEIGHTS_SIZE; i = i + 100)
		{
			printf("%p, ",ai_simplenn_data_weights_get()[i]);
		}
		printf("\n\nLast-1: %p",ai_simplenn_data_weights_get()[AI_SIMPLENN_DATA_WEIGHTS_SIZE-1]);
		printf("\nLast-2: %p",ai_simplenn_data_weights_get()[AI_SIMPLENN_DATA_WEIGHTS_SIZE-2]);*/
		/*if(*ai_simplenn_data_weights_get(void) == (uint8_t)0x7c) printf("\nEqual 0x7c\n");
		if(*(ai_simplenn_data_weights_get()+1) == (uint8_t)0x7c) printf("Equal1 0x7c\n");
		if(ai_simplenn_data_weights_get()[2] == (uint8_t)0x7c) printf("Equal2 0x7c\n");
		if(ai_simplenn_data_weights_get()[1] == (uint8_t) 0x26) printf("Equal 0x26\n");
		if(ai_simplenn_data_weights_get()[AI_SIMPLENN_DATA_WEIGHTS_SIZE-1] == (uint8_t)0x3d) printf("Equal 0x3d\n");
		if(ai_simplenn_data_weights_get()[AI_SIMPLENN_DATA_WEIGHTS_SIZE-2] == (uint8_t)0x64) printf("Equal 0x64\n");*/
		aiInit();
		nextState = RUNNING;
		printf("\nAI_INITED\n");
		cycleCounter = 0u;


		//osDelay(4000);
	}


}
float calculateReward(sensorSatate* data)
{
	/*Calculate current reward based on current states*/
	/* If emergency braking happens, reward is -100*/
	uint16_t lightLeft = 0u;
	uint16_t lightRight = 0u;
	float reward = 0;
	if(data->usLeft < 55u || data->usRight < 55u || data->laser < 550u)
	{
		return -100.0f;
	}
	else
	{
		lightRight = (uint16_t)data->laser;
		lightLeft = (uint16_t)(data->laser >> 8);
		/* left side*/
		for(uint8_t k = 0u; k < 4u; k++)
		{
			reward += (25.0f - k * 5.0f) * ((lightLeft >> k) & 0x0001);
		}
		for(uint8_t k = 4u; k < 31u; k++)
		{
			reward -= (k * 1.25f) * ((lightLeft >> k) & 0x0001);
		}

		/* right side*/
		for(uint8_t k = 0u; k < 4u; k++)
		{
			reward += (25.0f - k * 5.0f) * ((lightRight >> (31u - k)) & 0x0001);
		}
		for(uint8_t k = 4u; k < 31u; k++)
		{
			reward -= (k * 1.25f) * ((lightRight >> (31u - k)) & 0x0001);
		}
	}
	return reward;
}
void runningStateHandle(void)
{
	float avg = 0;
	uint32_t rNum;
	uint8_t request;
	/*Get the sensor values*/
	static sensorSatate previousStates;
	sensorSatate currentStates;
	float reward;
	HAL_StatusTypeDef status;
	float eps_threshold;
	uint8_t maxI = 0;
#ifdef AILOG
	char buf[100];
#endif
	/* Set the current state to the current sensor values*/

#ifdef TEST
	/* Set fake values*/
	printf("Fake values\n");
	currentStates.usLeft = 70u;
	currentStates.usRight = 80u;
	currentStates.laser = 750u;
	currentStates.lightsensor = 0x00018000;
	currentStates.leftM = 1.3f;
	currentStates.rightM = 2.1f;

	previousStates.reward = 100.23f;
	previousStates.usLeft = 78u;
	previousStates.usRight = 110u;
	previousStates.laser = 1234u;
	previousStates.lightsensor = 0x00019000;
	previousStates.leftM = 1.3f;
	previousStates.rightM = 2.4f;

#else
	printf("Real values\n");
	currentStates.usLeft = getUSDistanceLeft();;
	currentStates.usRight = getUSDistanceRight();
	currentStates.laser = getLaserDistance();
	currentStates.lightsensor = GetLightSensorValues();
#endif
	/* Calculate the reward of the previous actions based on the current state*/
	//reward = calculateReward(&currentStates);  // my solution
	reward = (float) GetReward();

#ifdef HAS_CAMERA
	request = ESP_REQUEST_CAMERA_FRAME;
	transmitToESP(&request, 1, HAL_MAX_DELAY);
	osDelay(10);
	status = receiveFromESP(camera_frame, CAMERA_WIDTH*CAMERA_HEIGHT, HAL_MAX_DELAY);
	if(HAL_OK == status) printf("Camera OK!!\n");
	if(camera_frame[0] == 0xEA) printf("0equals\n");
	if(camera_frame[0] == 0xEB) printf("0notequals\n");
	if(camera_frame[1] == 0x7C) printf("1astequals\n");
	if(camera_frame[CAMERA_WIDTH*CAMERA_HEIGHT-1] == 0xDB) printf("lastequals\n");
#endif

	if(cycleCounter > 0)
	{
		/*Send out state and action and reward*/
		/* State:previous sensor datas
		 * Action: previous action
		 * Reward: current reward calculated based on current sensor values which is the result of the previous states and actions*/
#ifdef LOGGING
		printf("Sending state to ESP\n");
        printf("-----------STATES---------\n");
        printf("lightsensor value Previous: %u\n", previousStates.lightsensor);
        printf("usLeft value Previous: %u\n", previousStates.usLeft);
        printf("usRight value Previous: %u\n", previousStates.usRight);
        printf("laser value Previous: %u\n", previousStates.laser);
        printf("leftM value Previous: %g\n", previousStates.leftM);
        printf("rightM sensor value Previous: %g\n", previousStates.rightM);
        printf("reward sensor value: %g\n", reward);
        printf("lightsensor value Current: %u\n", currentStates.lightsensor);
        printf("usLeft value Current: %u\n", currentStates.usLeft);
        printf("usRight value Current: %u\n", currentStates.usRight);
        printf("laser value Current: %u\n", currentStates.laser);
#endif
		request = ESP_SEND_STATE_1;
		transmitToESP(&request, 1, HAL_MAX_DELAY);
		osDelay(1);

		transmitToESP((uint8_t*)(&previousStates), 7*sizeof(uint32_t), HAL_MAX_DELAY);
		request = ESP_SEND_STATE_2;
		osDelay(1);
		transmitToESP(&request, 1, HAL_MAX_DELAY);
		osDelay(1);
		//transmitToESP((uint8_t*)(&reward), 4, HAL_MAX_DELAY);
		transmitToESP((uint8_t*)(&currentStates),4*sizeof(uint32_t) , HAL_MAX_DELAY);
		osDelay(1);

		printf("States transmitted\n");
	}

	for(uint32_t i = 0; i < AI_SIMPLENN_IN_1_SIZE; i++)
	{
		/*Normalize input data*/
		in_data[i] = ((float)camera_frame[i]) / (255.0f);
		avg += ((float)camera_frame[i]) / (CAMERA_WIDTH * CAMERA_HEIGHT);

	}
	printf("########### AVG: %g\n",avg);

	HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_SET);
	//HAL_Delay(2000);
	printf("Generating random number\n");
	/*Generate random number*/
	if(HAL_OK == HAL_RNG_GenerateRandomNumber(&hrng, &rNum))
	{
		printf("Random number generated: %g\n", rNum / ((float)0xFFFFFFFFu));
	}

	eps_threshold = EPS_END + (EPS_START - EPS_END) * expf(-1*(double)cycleCounter / EPS_DECAY);
	printf("Eps_threshold: %g\n",eps_threshold);
	/* 2 - Call inference engine */
	if((rNum / ((float)0xFFFFFFFFu)) < eps_threshold)
	{
		aiRun(in_data, out_data);
#ifdef AILOG
	//Send Data on Uart if necessary
	for(uint8_t i = 0u; i<AI_SIMPLENN_OUT_1_SIZE; i++)
	{
		printf("Output[%u]: ",i);
		printf("%g\n",out_data[i]);
	}
#endif
		/* Max search*/
		for(uint8_t k = 0; k < 4; k++)
		{
			if(out_data[k] > out_data[maxI])
			{
				maxI = k;
			}
		}
		printf("Max num: %g\n",out_data[maxI]);



	}
	else
	{
		HAL_RNG_GenerateRandomNumber(&hrng, &rNum);
		maxI = rNum % 4;
		printf("RandomMAX = %u\n",maxI);
	}
	switch(maxI)
			{
			case 0:
				currentStates.leftM = 1.0f;
				currentStates.rightM = 0.0f;
				leftMotor(1.0f  * LEFT_MOTOR_SPEED_SCALER);
				rightMotor(0.0f);
				break;
			case 1:
				currentStates.leftM = 0.0f;
				currentStates.rightM = 1.0f;
				rightMotor(1.0f  * RIGHT_MOTOR_SPEED_SCALER);
				leftMotor(0.0f);
				break;
			case 2:
				currentStates.leftM = 1.0f;
				currentStates.rightM = 1.0f;
				rightMotor(1.0f  * RIGHT_MOTOR_SPEED_SCALER);
				leftMotor(1.0f  * LEFT_MOTOR_SPEED_SCALER);
				break;
			case 3:
				currentStates.leftM = 0.0f;
				currentStates.rightM = 0.0f;
				rightMotor(0.0f  * RIGHT_MOTOR_SPEED_SCALER);
				leftMotor(0.0f  * LEFT_MOTOR_SPEED_SCALER);
				break;
			default:
				currentStates.leftM = 0.0f;
				currentStates.rightM = 0.0f;
				rightMotor(0.0f  * RIGHT_MOTOR_SPEED_SCALER);
				leftMotor(0.0f  * LEFT_MOTOR_SPEED_SCALER);
				break;
			}



	/* Set current actions */


	/* Copy current actions into previous state */
	previousStates.leftM = currentStates.leftM;
	previousStates.rightM = currentStates.rightM;

	/* Update the previous state*/
	previousStates.laser = currentStates.laser;
	previousStates.lightsensor = currentStates.lightsensor;
	previousStates.usLeft = currentStates.usLeft;
	previousStates.usRight = currentStates.usRight;

	HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);
	if(cycleCounter > MAX_CYCLES_0){
		nextState = LOAD;
	}
	else
	{
		cycleCounter++;
		nextState = RUNNING;
	}


}
