/*
 * lightSensor.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Kristof
 */

#include "lightSensor.h"

#define CMP_LVL 3500

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
		HAL_GPIO_WritePin(LED_LE_GPIO_Port, LED_LE_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED_LE_GPIO_Port, LED_LE_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(INF_LE_GPIO_Port, INF_LE_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(INF_LE_GPIO_Port, INF_LE_Pin, GPIO_PIN_RESET);
	}


}

uint16_t SPIreceive_AD(uint16_t idx, uint8_t AD_Num)
{
	uint16_t AD_OUT;
	uint16_t temp = idx << 11;
	uint16_t tmp = 0xFF00 & temp;
	temp <<= 8;
	tmp >>= 8;
	temp |= tmp;

	switch(AD_Num)
	{
		case 1:
			HAL_GPIO_WritePin(AD_CS1_GPIO_Port, AD_CS1_Pin, GPIO_PIN_RESET);
			break;

		case 2:
			HAL_GPIO_WritePin(AD_CS2_GPIO_Port, AD_CS2_Pin, GPIO_PIN_RESET);
			break;

		case 3:
			HAL_GPIO_WritePin(AD_CS3_GPIO_Port, AD_CS3_Pin, GPIO_PIN_RESET);
			break;

		case 4:
			HAL_GPIO_WritePin(AD_CS4_GPIO_Port, AD_CS4_Pin, GPIO_PIN_RESET);
			break;

		default: break;
	}

	while (!(SPI1->SR & SPI_SR_TXE));
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&temp, (uint8_t*)&AD_OUT, 2, 100);
	while (SPI1->SR & SPI_SR_BSY);

	switch(AD_Num)
	{
		case 1:
			HAL_GPIO_WritePin(AD_CS1_GPIO_Port, AD_CS1_Pin, GPIO_PIN_SET);
			break;

		case 2:
			HAL_GPIO_WritePin(AD_CS2_GPIO_Port, AD_CS2_Pin, GPIO_PIN_SET);
			break;

		case 3:
			HAL_GPIO_WritePin(AD_CS3_GPIO_Port, AD_CS3_Pin, GPIO_PIN_SET);
			break;

		case 4:
			HAL_GPIO_WritePin(AD_CS4_GPIO_Port, AD_CS4_Pin, GPIO_PIN_SET);
			break;

		default: break;
	}

	tmp = 0xFF00 & AD_OUT;
	AD_OUT <<= 8;
	tmp >>= 8;
	AD_OUT |= tmp;
	return AD_OUT;
}

void lightSensorCycle()
{
	uint8_t leds_buff[4]; //0->DxD, 1->DxC, 2->DxB, 3->DxA
	uint16_t AD_IN[32];
	uint8_t ledVal = 0x01;

	for(uint16_t i = 0; i < 8; i++)
	{
		//ledVal = 0x80; //DEBUGHOZ
		// IR LED write
		leds_buff[0] = leds_buff[1] = leds_buff[2] = leds_buff[3] = ledVal;
		SPItransmit_LED(leds_buff, 0);
		//SPItransmit_LED(leds_buff, 1); //DEBUGHOZ
		ledVal *= 2;

		// 140us delay??
		//HAL_Delay(1);

		// AD read (i, i+8, i+16, i+24)
		AD_IN[i] = (0xFFFF & SPIreceive_AD(i, 4)); //CS1
		AD_IN[i+8] = (0xFFFF & SPIreceive_AD(i, 3)); //CS2
		AD_IN[i+16] = (0xFFFF & SPIreceive_AD(i, 2)); //CS3
		AD_IN[i+24] = (0xFFFF & SPIreceive_AD(i, 1)); //CS4

		//HAL_Delay(500);
	}

	// Compare
	leds_buff[0] = leds_buff[1] = leds_buff[2] = leds_buff[3] = 0;
	for(int ledGroup = 0; ledGroup < 4; ledGroup++)
	{
		for(int ledNumInGroup = 0; ledNumInGroup < 8; ledNumInGroup++)
		{
			//if(ledGroup == 0 && ledNumInGroup == 7) //DEBUGHOZ
				printf("D%d%d: %u\n", 4-ledGroup, ledNumInGroup+1, AD_IN[8*ledGroup+ledNumInGroup]); //DEBUGHOZ
			if(AD_IN[8*ledGroup+ledNumInGroup] > CMP_LVL)
			{
				leds_buff[ledGroup] += (0x01 << ledNumInGroup);
			}
		}
	}

	// Debug LED write
	SPItransmit_LED(leds_buff, 1);
	//HAL_Delay(2000);
}

