/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "myCOM.h"
#include "encoder.h"
#include "motors.h"
#include "lightSensor.h"
#include "usSensor.h"
#include "lezerSensor.h"
#include "ACCSensor.h"
#include "reward.h"
#include "network.h"

#include "string.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define AD_CS1_Pin GPIO_PIN_2
#define AD_CS1_GPIO_Port GPIOE
#define AD_CS2_Pin GPIO_PIN_3
#define AD_CS2_GPIO_Port GPIOE
#define AD_CS3_Pin GPIO_PIN_4
#define AD_CS3_GPIO_Port GPIOE
#define AD_CS4_Pin GPIO_PIN_5
#define AD_CS4_GPIO_Port GPIOE
#define USER_BUTTON_Pin GPIO_PIN_13
#define USER_BUTTON_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define INF_LE_Pin GPIO_PIN_1
#define INF_LE_GPIO_Port GPIOB
#define INF_OE__Pin GPIO_PIN_2
#define INF_OE__GPIO_Port GPIOB
#define LED_LE_Pin GPIO_PIN_12
#define LED_LE_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD
#define ESP_RDY_Pin GPIO_PIN_11
#define ESP_RDY_GPIO_Port GPIOD
#define ESP_CS_Pin GPIO_PIN_12
#define ESP_CS_GPIO_Port GPIOD
#define USB_PowerSwitchOn_Pin GPIO_PIN_6
#define USB_PowerSwitchOn_GPIO_Port GPIOG
#define USB_OverCurrent_Pin GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port GPIOG
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define LED_OE__Pin GPIO_PIN_15
#define LED_OE__GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_7
#define LD2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define MOTOR_STOP 0.0
#define MOTOR_SLOWEST 0.45
#define MOTOR_SLOW 0.55
#define MOTOR_NORMAL 0.65
#define MOTOR_FAST 0.75
#define MOTOR_FASTEST 0.85
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
