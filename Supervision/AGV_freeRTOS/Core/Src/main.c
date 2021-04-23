/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "simplenn.h"
#include "simplenn_data.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
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
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
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
#define AILOG
#define TEST
#define LOGGING
#define HAS_CAMERA
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

CRC_HandleTypeDef hcrc;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

RNG_HandleTypeDef hrng;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart3;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for lightSensorTask */
osThreadId_t lightSensorTaskHandle;
const osThreadAttr_t lightSensorTask_attributes = {
  .name = "lightSensorTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for encodersTask */
osThreadId_t encodersTaskHandle;
const osThreadAttr_t encodersTask_attributes = {
  .name = "encodersTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal2,
};
/* Definitions for emergencyBreakingTask */
osThreadId_t emergencyBreakingTaskHandle;
const osThreadAttr_t emergencyBreakingTask_attributes = {
  .name = "emergencyBreakingTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for ACCTask */
osThreadId_t ACCTaskHandle;
const osThreadAttr_t ACCTask_attributes = {
  .name = "ACCTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal4,
};
/* Definitions for communicationTask */
osThreadId_t communicationTaskHandle;
const osThreadAttr_t communicationTask_attributes = {
  .name = "communicationTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal5,
};
/* Definitions for SemACC */
osSemaphoreId_t SemACCHandle;
const osSemaphoreAttr_t SemACC_attributes = {
  .name = "SemACC"
};
/* Definitions for SemMotors */
osSemaphoreId_t SemMotorsHandle;
const osSemaphoreAttr_t SemMotors_attributes = {
  .name = "SemMotors"
};
/* Definitions for SemLightSensor */
osSemaphoreId_t SemLightSensorHandle;
const osSemaphoreAttr_t SemLightSensor_attributes = {
  .name = "SemLightSensor"
};
/* Definitions for SemLeftEncoder */
osSemaphoreId_t SemLeftEncoderHandle;
const osSemaphoreAttr_t SemLeftEncoder_attributes = {
  .name = "SemLeftEncoder"
};
/* Definitions for SemRightEncoder */
osSemaphoreId_t SemRightEncoderHandle;
const osSemaphoreAttr_t SemRightEncoder_attributes = {
  .name = "SemRightEncoder"
};
/* Definitions for SemLeftUSSensor */
osSemaphoreId_t SemLeftUSSensorHandle;
const osSemaphoreAttr_t SemLeftUSSensor_attributes = {
  .name = "SemLeftUSSensor"
};
/* Definitions for SemRightUSSensor */
osSemaphoreId_t SemRightUSSensorHandle;
const osSemaphoreAttr_t SemRightUSSensor_attributes = {
  .name = "SemRightUSSensor"
};
/* Definitions for SemLaserSensor */
osSemaphoreId_t SemLaserSensorHandle;
const osSemaphoreAttr_t SemLaserSensor_attributes = {
  .name = "SemLaserSensor"
};
/* Definitions for SemPos */
osSemaphoreId_t SemPosHandle;
const osSemaphoreAttr_t SemPos_attributes = {
  .name = "SemPos"
};
/* Definitions for SemUSSensorEdge */
osSemaphoreId_t SemUSSensorEdgeHandle;
const osSemaphoreAttr_t SemUSSensorEdge_attributes = {
  .name = "SemUSSensorEdge"
};
/* USER CODE BEGIN PV */
// USSensor BEGIN
int motorDisable = 0;

extern uint32_t UStimeDifferenceLeft;
extern uint32_t USStartTimeLeft;
extern uint32_t USStopTimeLeft;

extern uint32_t UStimeDifferenceRight;
extern uint32_t USStartTimeRight;
extern uint32_t USStopTimeRight;

extern TIM_HandleTypeDef* UStim;

enum US_SENSOR{
	LEFT, RIGHT
};

volatile enum US_SENSOR currentUSSensor = LEFT;
// USSensor END


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
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM2_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_CRC_Init(void);
static void MX_RNG_Init(void);
static void MX_SPI3_Init(void);
void StartTaskDeafult(void *argument);
void StartTaskLightSensor(void *argument);
void StartTaskEncoders(void *argument);
void StartTaskEmergencyBreaking(void *argument);
void StartTaskACC(void *argument);
void StartTaskCommunication(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_I2C2_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_CRC_Init();
  MX_RNG_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  initMyCOM(&huart3);
  Encoder_Init();
  Motors_Init(&htim2);
  initUS(&htim3);
  initACCSensor(&hi2c2);
  initlezer(&htim1);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of SemACC */
  SemACCHandle = osSemaphoreNew(1, 1, &SemACC_attributes);

  /* creation of SemMotors */
  SemMotorsHandle = osSemaphoreNew(1, 1, &SemMotors_attributes);

  /* creation of SemLightSensor */
  SemLightSensorHandle = osSemaphoreNew(1, 1, &SemLightSensor_attributes);

  /* creation of SemLeftEncoder */
  SemLeftEncoderHandle = osSemaphoreNew(1, 1, &SemLeftEncoder_attributes);

  /* creation of SemRightEncoder */
  SemRightEncoderHandle = osSemaphoreNew(1, 1, &SemRightEncoder_attributes);

  /* creation of SemLeftUSSensor */
  SemLeftUSSensorHandle = osSemaphoreNew(1, 1, &SemLeftUSSensor_attributes);

  /* creation of SemRightUSSensor */
  SemRightUSSensorHandle = osSemaphoreNew(1, 1, &SemRightUSSensor_attributes);

  /* creation of SemLaserSensor */
  SemLaserSensorHandle = osSemaphoreNew(1, 1, &SemLaserSensor_attributes);

  /* creation of SemPos */
  SemPosHandle = osSemaphoreNew(1, 1, &SemPos_attributes);

  /* creation of SemUSSensorEdge */
  SemUSSensorEdgeHandle = osSemaphoreNew(1, 1, &SemUSSensorEdge_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartTaskDeafult, NULL, &defaultTask_attributes);

  /* creation of lightSensorTask */
  //lightSensorTaskHandle = osThreadNew(StartTaskLightSensor, NULL, &lightSensorTask_attributes);

  /* creation of encodersTask */
  //encodersTaskHandle = osThreadNew(StartTaskEncoders, NULL, &encodersTask_attributes);

  /* creation of emergencyBreakingTask */
  //emergencyBreakingTaskHandle = osThreadNew(StartTaskEmergencyBreaking, NULL, &emergencyBreakingTask_attributes);

  /* creation of ACCTask */
 // ACCTaskHandle = osThreadNew(StartTaskACC, NULL, &ACCTask_attributes);

  /* creation of communicationTask */
  //communicationTaskHandle = osThreadNew(StartTaskCommunication, NULL, &communicationTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00808CD2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00808CD2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2251;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 72-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 10;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 3599;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 3599;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 65535;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, AD_CS1_Pin|AD_CS2_Pin|AD_CS3_Pin|AD_CS4_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|INF_LE_Pin|INF_OE__Pin|LED_LE_Pin
                          |LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ESP_CS_GPIO_Port, ESP_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_OE__GPIO_Port, LED_OE__Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : AD_CS1_Pin AD_CS2_Pin AD_CS3_Pin AD_CS4_Pin */
  GPIO_InitStruct.Pin = AD_CS1_Pin|AD_CS2_Pin|AD_CS3_Pin|AD_CS4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA3 PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin INF_LE_Pin INF_OE__Pin LED_LE_Pin
                           LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|INF_LE_Pin|INF_OE__Pin|LED_LE_Pin
                          |LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : ESP_RDY_Pin */
  GPIO_InitStruct.Pin = ESP_RDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ESP_RDY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ESP_CS_Pin */
  GPIO_InitStruct.Pin = ESP_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ESP_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_OE__Pin */
  GPIO_InitStruct.Pin = LED_OE__Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_OE__GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

}

/* USER CODE BEGIN 4 */
int aiInit(void) {
  ai_error err;

#ifdef AILOG
  char buf[100];
  memset(buf,'\0',100);
#endif AILOG

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
      AI_SIMPLENN_DATA_WEIGHTS(s_simplenn_weights),//ai_simplenn_data_weights_get()),
      AI_SIMPLENN_DATA_ACTIVATIONS(activations) };

  if (!ai_simplenn_init(network, &params)) {
      err = ai_simplenn_get_error(network);
#ifdef AILOG
      memset(buf,'\0',100);
      sprintf(buf, "E: AI ai_network_init error - type=%d code=%d\r\n", err.type, err.code);
      HAL_Delay(10);
      HAL_UART_Transmit(&huart3, buf, 100, HAL_MAX_DELAY );
#endif AILOG
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
  };
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
		request = ESP_REQUEST_WEIGHTS;
		transmitToESP(&request, 1, HAL_MAX_DELAY);
		printf("Request sent\n");

		printf("Starting receiving weights\n");
		if(HAL_OK == receiveFromESP(s_simplenn_weights, sizeof(s_simplenn_weights), HAL_MAX_DELAY))
		{
			printf("Weights received successfully!!!\n");
			HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD2_Pin|LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD2_Pin, GPIO_PIN_SET);
		}
		else
		{
			printf("Weights reception failed!\n");
			HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD2_Pin|LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
				}
		aiInit();
		nextState = RUNNING;
		printf("AI_INITED\n");
		cycleCounter = 0u;
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
}
void runningStateHandle(void)
{
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
	reward = calculateReward(&currentStates);

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
		osDelay(5);

		transmitToESP((uint8_t*)(&previousStates), 7*sizeof(uint32_t), HAL_MAX_DELAY);
		request = ESP_SEND_STATE_2;
		osDelay(2);
		transmitToESP(&request, 1, HAL_MAX_DELAY);
		osDelay(2);
		//transmitToESP((uint8_t*)(&reward), 4, HAL_MAX_DELAY);
		transmitToESP((uint8_t*)(&currentStates),4*sizeof(uint32_t) , HAL_MAX_DELAY);
		osDelay(1);

		printf("States transmitted\n");
	}

	for(uint32_t i = 0; i < AI_SIMPLENN_IN_1_SIZE; i++)
	{
		/*Normalize input data*/
		in_data[i] = ((float)camera_frame[i]) / (255.0f);
	}

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
		nextState = INIT;
	}
	else
	{
		cycleCounter++;
		nextState = RUNNING;
	}


}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartTaskDeafult */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskDeafult */
void StartTaskDeafult(void *argument)
{
  /* USER CODE BEGIN 5 */
	/*
	leftMotor(0.6);
	rightMotor(0.6);
	uint32_t LEDs;
	uint16_t leftSide;
	uint16_t rightSide;
	int LLS, LS, RS, RRS;
	*/
	state = INIT;
	nextState = INIT;
  /* Infinite loop */
  for(;;)
  {
	  /*
	  if (!motorDisable)
	  {
		  LLS = LS = RS = RRS = 0;
		  LEDs = GetLightSensorValues();
		  rightSide = (uint16_t)(LEDs);
		  leftSide = (uint16_t)(LEDs >> 16);

		  // divide the sensors into 4 sectros
		  for(int i = 0; i < 8; i++)
		  {
			  RRS += (rightSide >> i) & 0x01;
			  LS += (leftSide >> i) & 0x01;
		  }
		  for(int i = 8; i < 16; i++)
		  {
			  RS += (rightSide >> i) & 0x01;
			  LLS += (leftSide >> i) & 0x01;
		  }

		  // compare the 4 sectors
		  if (LS + LLS > RS + RRS)
		  {
			  if (LLS > LS)
			  {
				  leftMotor(0.8);
				  rightMotor(0.45);
			  }
			  else if (LLS < LS)
			  {
				  leftMotor(0.6);
				  rightMotor(0.5);
			  }
		  }
		  else if (LS + LLS < RS + RRS)
		  {
			  if (RRS > RS)
			  {
				  leftMotor(0.45);
				  rightMotor(0.8);
			  }
			  else if (RRS < RS)
			  {
				  leftMotor(0.5);
				  rightMotor(0.6);
			  }
		  }
		  else
		  {
			  leftMotor(0.6);
			  rightMotor(0.6);
		  }
	  }
	  */
	  switch(state)
	  {
	  case INIT:
		  osDelay(4000);
		  printf("INIT state\n");
		  initStateHandle();
		  break;
	  case LOAD:
		  printf("LOAD state\n");
		  loadStateHandle();
		  break;

	  case RUNNING:
		  printf("RUNNING state\n");
		  runningStateHandle();
		  break;
	  default:
		  nextState = INIT;
	  }
	  state = nextState;

	  osDelay(CONTROL_DELAY);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTaskLightSensor */
/**
* @brief Function implementing the lightSensorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskLightSensor */
void StartTaskLightSensor(void *argument)
{
  /* USER CODE BEGIN StartTaskLightSensor */
  /* Infinite loop */
  for(;;)
  {
	  LightSensorCycle();
	  osDelay(10);
  }
  /* USER CODE END StartTaskLightSensor */
}

/* USER CODE BEGIN Header_StartTaskEncoders */
/**
* @brief Function implementing the encodersTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskEncoders */
void StartTaskEncoders(void *argument)
{
  /* USER CODE BEGIN StartTaskEncoders */
  int i = 0;

  /* Infinite loop */
  for(;;)
  {
	  CalculateDistance(LEFT_ENCODER);
	  CalculateDistance(RIGHT_ENCODER);
	  if (i % 10 == 0)
	  {
		  CalculateSpeed(LEFT_ENCODER);
		  CalculateSpeed(RIGHT_ENCODER);
	  }
	  else if (i == 50)
	  {
		  CalculatePositionAndAngle();
		  i = 0;
	  }
	  else
	  {
		  i++;
	  }

	  osDelay(10);
  }
  /* USER CODE END StartTaskEncoders */
}

/* USER CODE BEGIN Header_StartTaskEmergencyBreaking */
/**
* @brief Function implementing the emergencyBreakingTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskEmergencyBreaking */
void StartTaskEmergencyBreaking(void *argument)
{
  /* USER CODE BEGIN StartTaskEmergencyBreaking */
  unsigned int lDist = 500;
  unsigned int rDist = 500;
  unsigned int mDist = 5000;
  /* Infinite loop */
  for(;;)
  {
	  lDist = (unsigned int)getUSDistanceLeft();
	  rDist = (unsigned int)getUSDistanceRight();
	  mDist = (unsigned int)getLaserDistance();

	  if (lDist < 50 || rDist < 50 || mDist < 500)
	  {
		  motorDisable = 1;
		  leftMotor(0);
		  rightMotor(0);
	  }
	  else
	  {
		  motorDisable = 0;
	  }

	  osDelay(10);
  }
  /* USER CODE END StartTaskEmergencyBreaking */
}

/* USER CODE BEGIN Header_StartTaskACC */
/**
* @brief Function implementing the ACCTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskACC */
void StartTaskACC(void *argument)
{
  /* USER CODE BEGIN StartTaskACC */
	setACC_OS_RUNNING(1);
  /* Infinite loop */
  for(;;)
  {
	 AccMeasure();
	 GyroMeasure();
	 EulerMeasure();
	 osDelay(10);
  }
  /* USER CODE END StartTaskACC */
}

/* USER CODE BEGIN Header_StartTaskCommunication */
/**
* @brief Function implementing the communicationTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskCommunication */
void StartTaskCommunication(void *argument)
{
  /* USER CODE BEGIN StartTaskCommunication */
  /* Infinite loop */
  for(;;)
  {
	  /*
	  printf("\033[3J");
	  printf("\e[1;1H\e[2J");
	  printf("Light sensor: 0x%x\n", GetLightSensorValues());
	  printf("Left US sensor: %u\n", getUSDistanceLeft());
	  printf("Right US sensor: %u\n", getUSDistanceRight());
	  printf("Laser sensor: %u\n", getlezerDistance());
	  printf("Acceleration: x: %f y: %f z: %f, Euler: x: %f y: %f z: %f\n", getAcc().x, getAcc().y, getAcc().z, getEuler().x, getEuler().y, getEuler().z);
	  GetEncoderData();
	  */

	  osDelay(1000);

  }
  /* USER CODE END StartTaskCommunication */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM10 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM10) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  	//Left encoder BEGIN
	if(htim->Instance == TIM6)
	{
		DisableSpeed(LEFT_ENCODER);
	}
	//Left encoder END

	//Right encoder BEGIN
	if(htim->Instance == TIM7)
	{
		DisableSpeed(RIGHT_ENCODER);
	}
	//Right encoder END

  	//US Sensor BEGIN
  	if(htim->Instance == UStim->Instance){
  		//LEFT
  		if(currentUSSensor == LEFT){
  			HAL_TIM_Base_Stop_IT(UStim);
  			//Stop LEFT
  			HAL_TIM_PWM_Stop(UStim, TIM_CHANNEL_4);
  			HAL_TIM_IC_Stop_IT(UStim, TIM_CHANNEL_3);
  			//If echo not detected, distance set to 500 cm
  			if(getUSRisingEdgeLeftCallback() == 1) setUSDistanceLeftCallBack(500);
  			setUSRisingEdgeLeftCallBack(0);
  			//Set timer to zero
  			__HAL_TIM_SET_COUNTER(UStim, 0);
  			//Set RIGHT
  			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
  			//Start RIGHT
  			HAL_TIM_IC_Start_IT(UStim, TIM_CHANNEL_1);
  			HAL_TIM_PWM_Start(UStim, TIM_CHANNEL_2);
  			HAL_TIM_Base_Start_IT(UStim);

  			currentUSSensor = RIGHT;
  		}
  		//RIGHT
  		else if(currentUSSensor == RIGHT){
  			HAL_TIM_Base_Stop_IT(UStim);
  			//Stop RIGHT
  			HAL_TIM_PWM_Stop(UStim, TIM_CHANNEL_2);
  			HAL_TIM_IC_Stop_IT(UStim, TIM_CHANNEL_1);
  			//If echo not detected, distance set to 500 cm
  			if(getUSRisingEdgeRightCallback() == 1) setUSDistanceRightCallBack(500);
  			setUSRisingEdgeRightCallBack(0);
  			//Set timer to zero
  			__HAL_TIM_SET_COUNTER(UStim, 0);
  			//Set LEFT
  			__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_3, TIM_INPUTCHANNELPOLARITY_RISING);
  			//Start LEFT
  			HAL_TIM_IC_Start_IT(UStim, TIM_CHANNEL_3);
  			HAL_TIM_PWM_Start(UStim, TIM_CHANNEL_4);
  			HAL_TIM_Base_Start_IT(UStim);

  			currentUSSensor = LEFT;
  		}
  	}
  	//US Sensor END
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
