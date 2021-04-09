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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

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
/* USER CODE BEGIN PV */
// USSensor BEGIN
int motorDisable = 0;

extern uint32_t UStimeDifferenceLeft;
extern uint32_t USStartTimeLeft;
extern uint32_t USStopTimeLeft;
extern uint32_t USdistanceLeft;
extern uint8_t USrisingEdgeDetectedLeft;

extern uint32_t UStimeDifferenceRight;
extern uint32_t USStartTimeRight;
extern uint32_t USStopTimeRight;
extern uint32_t USdistanceRight;
extern uint8_t USrisingEdgeDetectedRight;

extern TIM_HandleTypeDef* UStim;

enum US_SENSOR{
	LEFT, RIGHT
};

volatile enum US_SENSOR currentUSSensor = LEFT;
// USSensor END

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
  lightSensorTaskHandle = osThreadNew(StartTaskLightSensor, NULL, &lightSensorTask_attributes);

  /* creation of encodersTask */
  encodersTaskHandle = osThreadNew(StartTaskEncoders, NULL, &encodersTask_attributes);

  /* creation of emergencyBreakingTask */
  emergencyBreakingTaskHandle = osThreadNew(StartTaskEmergencyBreaking, NULL, &emergencyBreakingTask_attributes);

  /* creation of ACCTask */
  ACCTaskHandle = osThreadNew(StartTaskACC, NULL, &ACCTask_attributes);

  /* creation of communicationTask */
  communicationTaskHandle = osThreadNew(StartTaskCommunication, NULL, &communicationTask_attributes);

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
                              |RCC_PERIPHCLK_I2C2;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
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
	leftMotor(0.6);
	rightMotor(0.6);
	uint32_t LEDs;
	uint16_t leftSide;
	uint16_t rightSide;
	int LLS, LS, RS, RRS;
  /* Infinite loop */
  for(;;)
  {
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
	  osDelay(10);
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
	  mDist = (unsigned int)getlezerDistance();

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
  /* Infinite loop */
  for(;;)
  {
	 //AccMeasure();
	 //GyroMeasure();
	 //EulerMeasure();
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
  			if(USrisingEdgeDetectedLeft == 1) USdistanceLeft = 500;
  			USrisingEdgeDetectedLeft = 0;
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
  			if(USrisingEdgeDetectedRight == 1) USdistanceRight = 500;
  			USrisingEdgeDetectedRight = 0;
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
