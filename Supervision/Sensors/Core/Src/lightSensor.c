/*
 * lightSensor.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Kristof
 */

#include "lightSensor.h"

#define CMP_LVL 999

// ledType: IR = 0; Debug LED = 1;
void SPItransmit_LED(uint8_t* leds, int ledType)
{
	// Check if the SPI is enabled
	if((SPI2->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE)
	{
		// If disabled, I enable it
		SET_BIT(SPI2->CR1, SPI_CR1_SPE);
	}

	// Wait until SPI is ready
	while (!(SPI2->SR & SPI_SR_TXE));

	// Send bytes over the SPI
	HAL_SPI_Transmit(&hspi2, leds, 4, 100);
	// Wait until the transmission is complete
	while (SPI2->SR & SPI_SR_BSY);

	/*// Disable SPI
	CLEAR_BIT(SPI2->CR1, SPI_CR1_SPE);*/

	if(ledType)
	{
		HAL_GPIO_WritePin(GPIOB, LED_LE_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, LED_LE_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, INF_LE_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, INF_LE_Pin, GPIO_PIN_RESET);
	}


}

uint16_t SPIreceive_AD(uint16_t idx, uint8_t AD_Num)
{
	uint16_t AD_OUT;
	uint16_t temp = idx << 11;

	switch(AD_Num)
	{
		case 1:
			HAL_GPIO_WritePin(GPIOB, AD_CS1_Pin, GPIO_PIN_RESET);
			break;

		case 2:
			HAL_GPIO_WritePin(GPIOB, AD_CS2_Pin, GPIO_PIN_RESET);
			break;

		case 3:
			HAL_GPIO_WritePin(GPIOB, AD_CS3_Pin, GPIO_PIN_RESET);
			break;

		case 4:
			HAL_GPIO_WritePin(GPIOB, AD_CS4_Pin, GPIO_PIN_RESET);
			break;

		default: break;
	}

	while (!(SPI1->SR & SPI_SR_TXE));
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&temp, (uint8_t*)&AD_OUT, 2, 100);
	while (SPI1->SR & SPI_SR_BSY);

	switch(AD_Num)
	{
		case 1:
			HAL_GPIO_WritePin(GPIOB, AD_CS1_Pin, GPIO_PIN_SET);
			break;

		case 2:
			HAL_GPIO_WritePin(GPIOB, AD_CS2_Pin, GPIO_PIN_SET);
			break;

		case 3:
			HAL_GPIO_WritePin(GPIOB, AD_CS3_Pin, GPIO_PIN_SET);
			break;

		case 4:
			HAL_GPIO_WritePin(GPIOB, AD_CS4_Pin, GPIO_PIN_SET);
			break;

		default: break;
	}

	return AD_OUT;
}

void lightSensorCycle()
{
	uint8_t leds_buff[4];
	uint16_t AD_IN[32];
	uint8_t ledVal = 0x01;

	for(uint16_t i = 0; i < 8; i++)
	{
		// IR LED write
		leds_buff[0] = leds_buff[1] = leds_buff[2] = leds_buff[3] = ledVal;
		SPItransmit_LED(leds_buff, 0);
		ledVal *= 2;

		// 140us delay??
		HAL_Delay(1);

		// AD read (i, i+8, i+16, i+24)
		AD_IN[i] = SPIreceive_AD(i, 1);;
		AD_IN[i+8] = SPIreceive_AD(i, 2);
		AD_IN[i+16] = SPIreceive_AD(i, 3);
		AD_IN[i+24] = SPIreceive_AD(i, 4);
	}

	// Compare
	for(int ledGroup = 0; ledGroup < 4; ledGroup++)
	{
		for(int ledNumInGroup = 0; ledNumInGroup < 8; ledNumInGroup++)
		{
			printf("%u\n", AD_IN[8*ledGroup+ledNumInGroup]);
			if(AD_IN[8*ledGroup+ledNumInGroup] < CMP_LVL)
			{
				leds_buff[ledGroup] += ledNumInGroup;
			}
		}
	}

	// Debug LED write
	SPItransmit_LED(leds_buff, 1);
}

