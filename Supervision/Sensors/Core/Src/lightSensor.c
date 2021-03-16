/*
 * lightSensor.c
 *
 *  Created on: Mar 10, 2021
 *      Author: Kristof
 */

#include "lightSensor.h"

#define CMP_LVL 3650

// ledType: IR = 0; Debug LED = 1;
void SPItransmit_LED(uint8_t* leds, int ledType)
{
	// Wait until SPI is ready
	while (!(SPI2->SR & SPI_SR_TXE));
	// Send bytes over the SPI
	//HAL_SPI_Transmit(&hspi2, leds, 4, 100);
	//printf("Transmit OUT: 4: %x, 3: %x, 2: %x, 1: %x\n", leds[0], leds[1], leds[2], leds[3]);
	HAL_SPI_Transmit(&hspi2, &leds[0], 1, 100);
	HAL_SPI_Transmit(&hspi2, &leds[1], 1, 100);
	HAL_SPI_Transmit(&hspi2, &leds[2], 1, 100);
	HAL_SPI_Transmit(&hspi2, &leds[3], 1, 100);
	// Wait until the transmission is complete
	while (SPI2->SR & SPI_SR_BSY);

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
	//idx <<= 11;
	//printf("%u\n", idx);
	//switchBytes(&idx); // Ez nem biztos h kell, tesztelni kell majd
	//printf("%u\n", idx);

	//TEST CASE
	uint8_t buff_out[2];
	buff_out[0] = (uint8_t)(idx << 3);
	buff_out[1] = (uint8_t)(idx << 3);
	//printf("AD: %u, LED: %u, ", AD_Num, idx+1);

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
	HAL_SPI_TransmitReceive(&hspi1, buff_out, (uint8_t*)&AD_OUT, 2, 100); //DEBUGHOZ
	//HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)&idx, (uint8_t*)&AD_OUT, 2, 100);
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

	switchBytes(&AD_OUT);
	printf("Out: %u\n", AD_OUT);
	return AD_OUT;
}

void lightSensorCycle()
{
	uint8_t leds_buff[4]; //0 -> DxD, 1 -> DxC, 2 -> DxB, 3 -> DxA
	uint16_t AD_IN[32];
	uint8_t ledVal = 0x01;

	for(uint16_t i = 0; i < 8; i++)
	{
		// IR LED write
		leds_buff[0] = leds_buff[1] = leds_buff[2] = leds_buff[3] = ledVal;
		//printf("IR: 4: %x, 3: %x, 2: %x, 1: %x\n", leds_buff[0], leds_buff[1], leds_buff[2], leds_buff[3]);
		SPItransmit_LED(leds_buff, 0);
		//SPItransmit_LED(leds_buff, 1); //DEBUGHOZ
		ledVal <<= 1;

		// 140us delay szükséges??
		HAL_Delay(1);

		// AD read (i -> group4(24-31), i+8 -> group3(16-23), i+16 -> group2(8-15), i+24 -> group1(0-7))
		AD_IN[i] 	= SPIreceive_AD(i, 4); //CS4 Group4
		//HAL_Delay(1);
		AD_IN[i+8] 	= SPIreceive_AD(i, 3); //CS3 Group3
	//	HAL_Delay(1);
		AD_IN[i+16] = SPIreceive_AD(i, 2); //CS2 Group2
	//	HAL_Delay(1);
		AD_IN[i+24] = SPIreceive_AD(i, 1); //CS1 Group1
	}

	// Compare
	leds_buff[0] = leds_buff[1] = leds_buff[2] = leds_buff[3] = 0;
	for(int ledGroup = 0; ledGroup < 4; ledGroup++)
	{
		for(int ledNumInGroup = 0; ledNumInGroup < 8; ledNumInGroup++)
		{
			//if(ledGroup == 0 && ledNumInGroup == 7) //DEBUGHOZ
				//printf("D%d%c: %u\n", ledNumInGroup+1, 'A'+3-ledGroup, AD_IN[8*ledGroup+ledNumInGroup]); //DEBUGHOZ
			if(AD_IN[8*ledGroup+ledNumInGroup] > CMP_LVL)
			{
				leds_buff[ledGroup] |= (1 << ledNumInGroup);
			}
		}
	}


	// Debug LED write
	SPItransmit_LED(leds_buff, 1);
	//HAL_Delay(500);
}

void switchBytes(uint16_t* num)
{
	uint16_t tmp = 0xFF00 & (*num);
	(*num) <<= 8;
	tmp >>= 8;
	(*num) |= tmp;
}
